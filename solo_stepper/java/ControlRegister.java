package drv8711;
import org.junit.Test;

public class ControlRegister {

	public enum Controller {
		Disabled, Enabled
	};

	public enum Direction {
		Normal, Reverse
	};

	public enum StallDetection {
		Internal, External
	};

	public enum AmpGaines {
		Gain_x5, Gain_x10, Gain_x20, Gain_x40
	}

	public enum DeadTimes {
		T400ns, T450ns, T650ns, T850ns
	}

	public enum StepModes {
		FullStep, HalfStep, Step4, Step8, Step16, Step32, Step64, Step128, Step256
	};

	private Controller nController = Controller.Disabled;
	private Direction nDirection = Direction.Normal;
	private StallDetection nDetection = StallDetection.Internal;
	private AmpGaines nAmpGain = AmpGaines.Gain_x5;
	private DeadTimes nDeadTime = DeadTimes.T400ns;
	private StepModes nStepMode = StepModes.FullStep;

	@Test
	public void step() {
		System.out.println("s");
	}

	@Test
	public void set(Controller controller, Direction direction, StallDetection detection, AmpGaines ampGain,
			DeadTimes deadTime, StepModes stepMode) {
		nController = controller;
		nDirection = direction;
		nDetection = detection;
		nAmpGain = ampGain;
		nDeadTime = deadTime;
		nStepMode = stepMode;
		printAttributes();
	}

	public void printAttributes() {
		System.out.print(nController + " ");
		System.out.print(nDirection + " ");
		System.out.print(nDetection + " ");
		System.out.print(nAmpGain + " ");
		System.out.print(nDeadTime + " ");
		System.out.println(nStepMode);
	}
}
