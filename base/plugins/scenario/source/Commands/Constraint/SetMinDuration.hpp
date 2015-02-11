#pragma once
#include <core/presenter/command/SerializableCommand.hpp>
#include <tools/ObjectPath.hpp>

#include <tests/helpers/ForwardDeclaration.hpp>
namespace Scenario
{
	namespace Command
	{
		/**
		 * @brief The SetMinDuration class
		 *
		 * Sets the Min duration of a Constraint
		 */
		class SetMinDuration : public iscore::SerializableCommand
		{
#include <tests/helpers/FriendDeclaration.hpp>

			public:
				SetMinDuration();
				SetMinDuration(ObjectPath&& constraintPath, int duration);

				virtual void undo() override;
				virtual void redo() override;
				virtual int id() const override;
				virtual bool mergeWith(const QUndoCommand* other) override;

			protected:
				virtual void serializeImpl(QDataStream&) const override;
				virtual void deserializeImpl(QDataStream&) override;

			private:
				ObjectPath m_path;

				int m_oldDuration{};
				int m_newDuration{};
		};
	}
}
