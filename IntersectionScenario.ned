import org.car2x.veins.nodes.RSU;
import org.car2x.veins.nodes.Scenario;

network IntersectionScenario extends Scenario
{
    submodules:
        rsu[1]: RSU {
        }
}