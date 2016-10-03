#include "StringConstants.hpp"

namespace iscore
{
ISCORE_LIB_BASE_EXPORT const StringConstants& StringConstant()
{
    static const StringConstants strings{
                QStringLiteral("k"),
                QStringLiteral("v"),
                QStringLiteral("id"),
                QStringLiteral("none"),
                QStringLiteral("Identifiers"),
                QStringLiteral("Type"),
                QStringLiteral("Value"),
                QStringLiteral("Address"),
                QStringLiteral("Message"),
                QStringLiteral("value"),
                QStringLiteral("address"),
                QStringLiteral("LHS"),
                QStringLiteral("Op"),
                QStringLiteral("RHS"),
                QStringLiteral("Previous"),
                QStringLiteral("Following"),
                QStringLiteral("User"),
                QStringLiteral("Priorities"),
                QStringLiteral("Process"),
                QStringLiteral("Name"),
                QStringLiteral("ObjectName"),
                QStringLiteral("ObjectId"),
                QStringLiteral("Children"),
                QStringLiteral("Min"),
                QStringLiteral("Max"),
                QStringLiteral("Values"),
                QStringLiteral("Device"),
                QStringLiteral("Path"),
                QStringLiteral("ioType"),
                QStringLiteral("ClipMode"),
                QStringLiteral("Unit"),
                QStringLiteral("RepetitionFilter"),
                QStringLiteral("RefreshRate"),
                QStringLiteral("Priority"),
                QStringLiteral("Tags"),
                QStringLiteral("Domain"),
                QStringLiteral("Protocol"),
                QStringLiteral("Duration"),
                QStringLiteral("Metadata"),
                QStringLiteral("true"),
                QStringLiteral("false"),
                QStringLiteral("True"),
                QStringLiteral("False"),
                QStringLiteral("yes"),
                QStringLiteral("no"),
                QStringLiteral("Yes"),
                QStringLiteral("No"),
                QStringLiteral("Start"),
                QStringLiteral("End"),
                QStringLiteral("ScriptingName"),
                QStringLiteral("Comment"),
                QStringLiteral("Color"),
                QStringLiteral("Label"),
                QStringLiteral("Extended"),
                QStringLiteral("uuid"),
                QStringLiteral("Description")
    };
    return strings;
}

}
