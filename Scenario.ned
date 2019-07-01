import org.car2x.veins.nodes.RSU;
import org.car2x.veins.nodes.Scenario;

network BaseScenario extends Scenario
{
    submodules:
        rsu[1]: RSU {
            @display("p=150,150;i=veins/sign/yellowdiamond;is=vs");
        }
}