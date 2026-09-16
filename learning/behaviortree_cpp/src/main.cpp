#include "behaviortree_cpp/bt_factory.h"

class SayHello : public BT::SyncActionNode
{
public:
  SayHello(const std::string& name, const BT::NodeConfig& config)
    : BT::SyncActionNode(name, config)
  {
  }

  static BT::PortsList providedPorts()
  {
    return {};
  }

  BT::NodeStatus tick() override
  {
    std::cout << "Hello!" << std::endl;
    return BT::NodeStatus::SUCCESS;
  }
};

static const char* xml_tree = R"(
 <root BTCPP_format="4">
   <BehaviorTree ID="MainTree">
    <Sequence name="root_sequence">
      <SayHello />
    </Sequence>
   </BehaviorTree>
 </root>
)";

int main()
{
  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<SayHello>("SayHello");

  auto tree = factory.createTreeFromText(xml_tree);
  while (true)
  {
    tree.tickOnce();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}
