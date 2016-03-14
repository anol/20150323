package drv8711;

import static org.junit.Assert.fail;

import org.junit.Test;

public class TorqueRegister {

	private int nTorque = 0;
	private int nThreshold = 0;

	@Test
	public void set(int torque, int threshold) {
		nTorque = torque;
		nThreshold = threshold;
		printAttributes();
		if ((0 > torque) || (255 < torque) || (50 > threshold) || (6400 < threshold)) {
			fail("Wrong values");
		}
	}

	public void printAttributes() {
		System.out.println("T=" + nTorque + "ET=" + nThreshold);
	}
}
