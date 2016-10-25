#include <Scenario/Commands/Cohesion/CreateCurveFromStates.hpp>
#include <Scenario/Commands/Cohesion/InterpolateMacro.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <Explorer/Explorer/DeviceExplorerModel.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/State/ItemModel/MessageItemModel.hpp>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Process/State/MessageNode.hpp>
#include <QList>
#include <QPointer>

#include "CreateCurves.hpp"
#include <Device/Node/DeviceNode.hpp>
#include <Device/Address/AddressSettings.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/selection/Selectable.hpp>
#include <iscore/selection/SelectionStack.hpp>
#include <iscore/tools/IdentifiedObjectAbstract.hpp>

#include <Automation/AutomationModel.hpp>
#include <ossia/editor/value/value_conversion.hpp>
namespace Scenario
{
static std::vector<Device::FullAddressSettings> getSelectedAddresses(
        const iscore::DocumentContext& doc)
{
    // First get the addresses
    auto& device_explorer = Explorer::deviceExplorerFromContext(doc);

    std::vector<Device::FullAddressSettings> addresses;
    for(const auto& index : device_explorer.selectedIndexes())
    {
        const auto& node = device_explorer.nodeFromModelIndex(index);
        if(node.is<Device::AddressSettings>())
        {
            const Device::AddressSettings& addr = node.get<Device::AddressSettings>();
            if(addr.value.val.isNumeric())
            {
                Device::FullAddressSettings as;
                static_cast<Device::AddressSettingsCommon&>(as) = addr;
                as.address = Device::address(node).address;
                addresses.push_back(std::move(as));
            }
        }
    }
    return addresses;

}

void CreateCurves(
        const QList<const Scenario::ConstraintModel*>& selected_constraints,
        const iscore::CommandStackFacade& stack)
{
    if(selected_constraints.empty())
        return;

    // For each constraint, interpolate between the states in its start event and end event.
    auto& doc = iscore::IDocument::documentContext(*selected_constraints.first());

    auto addresses = getSelectedAddresses(doc);
    if(addresses.empty())
        return;

    CreateCurvesFromAddresses(selected_constraints, addresses, stack);
}

void CreateCurvesFromAddresses(
        const QList<const Scenario::ConstraintModel*>& selected_constraints,
        const std::vector<Device::FullAddressSettings>& addresses,
        const iscore::CommandStackFacade& stack)
{
    if(selected_constraints.empty())
        return;

    // They should all be in the same scenario so we can select the first.
    // FIXME check that the other "cohesion" methods also use ScenarioInterface and not Scenario::ProcessModel
    auto scenar = dynamic_cast<Scenario::ScenarioInterface*>(
                                selected_constraints.first()->parent());


    int added_processes = 0;
    // Then create the commands
    auto big_macro = new Scenario::Command::AddMultipleProcessesToMultipleConstraintsMacro;

    for(const auto& constraint_ptr : selected_constraints)
    {
        auto& constraint = *constraint_ptr;
        // Generate brand new ids for the processes
        auto process_ids = getStrongIdRange<Process::ProcessModel>(addresses.size(), constraint.processes);
        auto macro_tuple = Scenario::Command::makeAddProcessMacro(constraint, addresses.size());
        auto macro = std::get<0>(macro_tuple);
        auto& bigLayerVec = std::get<1>(macro_tuple);

        const Scenario::StateModel& ss = startState(constraint, *scenar);
        const auto& es = endState(constraint, *scenar);

        std::vector<State::AddressAccessor> existing_automations;
        for(const auto& proc : constraint.processes)
        {
            if(auto autom = dynamic_cast<const Automation::ProcessModel*>(&proc))
                existing_automations.push_back(autom->address());
        }

        int i = 0;
        for(const Device::FullAddressSettings& as : addresses)
        {
            // First, we skip the curve if there is already a curve
            // with this address in the constraint.
            if(ossia::contains(existing_automations, State::AddressAccessor{as.address}))
                continue;

            // Then we set-up all the necessary values
            // min / max
            auto min_v = as.domain.get_min();
            auto max_v = as.domain.get_max();
            double min = ossia::is_numeric(min_v)
                    ? ossia::convert<double>(min_v)
                    : 0;

            double max = ossia::is_numeric(max_v)
                    ? ossia::convert<double>(max_v)
                    : 1;

            // start value / end value
            double start = std::min(min, max);
            double end = std::max(min, max);
            Process::MessageNode* s_node = Device::try_getNodeFromString(
                        ss.messages().rootNode(),
                        stringList(as.address));
            if(s_node)
            {
                if(auto val = s_node->value())
                {
                    start = State::convert::value<double>(*val);
                    min = std::min(start, min);
                    max = std::max(start, max);
                }
            }

            Process::MessageNode* e_node = Device::try_getNodeFromString(
                        es.messages().rootNode(),
                        stringList(as.address));
            if(e_node)
            {
                if(auto val = e_node->value())
                {
                    end = State::convert::value<double>(*val);
                    min = std::min(end, min);
                    max = std::max(end, max);
                }
            }

            // Send the command.
            macro->addCommand(new Scenario::Command::CreateAutomationFromStates{
                                  constraint,
                                  bigLayerVec[i],
                                  process_ids[i],
                                  State::AddressAccessor{as.address},
                                  start, end, min, max
                              });

            i++;
            added_processes++;
        }
        big_macro->addCommand(macro);
    }

    if(added_processes > 0)
    {
        CommandDispatcher<> disp{stack};
        disp.submitCommand(big_macro);
    }
    else
    {
        delete big_macro;
    }
}
}
