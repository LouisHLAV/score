#pragma once
#include <Scenario/Document/State/StateModel.hpp>
#include <Network/Node.h>
#include <OSSIA/LocalTree/BaseProperty.hpp>
#include <OSSIA/LocalTree/LocalTreeDocumentPlugin.hpp>
#include <OSSIA/LocalTree/NameProperty.hpp>

namespace OSSIA
{
namespace LocalTree
{
class StateComponent final :
        public iscore::Component
{
    public:
        using system_t = OSSIA::LocalTree::DocumentPlugin;

        StateComponent(
                OSSIA::Node& parent,
                const Id<iscore::Component>& id,
                StateModel& state,
                const system_t& doc,
                const iscore::DocumentContext& ctx,
                QObject* parent_comp);

        const Key& key() const override;

        auto& node() const
        { return m_thisNode.node; }
        ~StateComponent();

    private:
        OSSIA::Node& thisNode() const
        { return *node(); }

        MetadataNamePropertyWrapper m_thisNode;
        std::vector<BaseProperty*> m_properties;

};
}
}
