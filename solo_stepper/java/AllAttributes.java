package drv8711;

public class AllAttributes {

	public static final Attribute[] Drv8711Attributes = {

			new Attribute(0, 0, 1, "ENBL", "Enable the DRV8711 Stepper Controller", "Disable, Enable"),
			new Attribute(0, 1, 1, "RDIR", "Reverse direction", "Normal, Reverse"),
			new Attribute(0, 2, 1, "RSTEP", "Advance indexer one step", ", Step"),
			new Attribute(0, 3, 4, "MODE", "Step mode",
					"FullStep, HalfStep, Step4, Step8, Step16, Step32, Step64, Step128, Step256"),
			new Attribute(0, 7, 1, "EXSTALL", "Stall detection", "Internal, External"),
			new Attribute(0, 8, 2, "ISGAIN", "Amplifier gain", "Gain_x5, Gain_x10, Gain_x20, Gain_x40"),
			new Attribute(0, 10, 2, "DTIME", "Dead time", "T400ns, T450ns, T650ns, T850ns"),
			new Attribute(1, 0, 8, "TORQUE", "Full-scale output current", "0..255"),
			new Attribute(1, 8, 3, "SMPLTH", "Back EMF sample threshold",
					"50us, 100us, 200us, 300us, 400us, 600us, 800us, 1000us"),
			new Attribute(2, 0, 8, "TOFF", "Fixed off time: 500ns + N*500ns", "0..255"),
			new Attribute(2, 8, 1, "PWMMODE", "Indexer", "Normal|Bypass"),
			new Attribute(3, 0, 8, "TBLANK", "Current trip blanking time: 1us + N*20ns", "0..255"),
			new Attribute(3, 8, 1, "ABT", "Adaptive blanking time", "Disable, Enable"),
			new Attribute(4, 0, 8, "TDECAY", "Mixed decay transition time: N*500ns", "0..255"),
			new Attribute(4, 8, 3, "DECMOD", "Decay mode", "Slow, Force, Mixed, Fast, Auto"),
			new Attribute(5, 0, 8, "SDTHR", "Stall detect threshold", "0..255"),
			new Attribute(5, 8, 2, "SDCNT", "Stall detect asserted count-down", "1, 2, 4, 8"),
			new Attribute(5, 10, 2, "VDIV", "Back EMF divider", "1/32, 1/16, 1/8, 1/4"),
			new Attribute(6, 0, 2, "OCPTH", "OCP threshold", "250mV, 500mV, 750mV, 1000mV"),
			new Attribute(6, 2, 2, "OCPDEG", "OCP deglitch time", "1us, 2us, 4us, 8us"),
			new Attribute(6, 4, 2, "TDRIVEN", "Low-side gate drive time", "250ns, 500ns, 1us, 2us"),
			new Attribute(6, 6, 2, "TDRIVEP", "High-side gate drive time", "250ns, 500ns, 1us, 2us"),
			new Attribute(6, 8, 2, "IDRIVEN", "Low-side gate drive peak current", "100mA, 200mA, …"),
			new Attribute(6, 10, 2, "IDRIVEP", "High-side gate drive peak current", "50mA, 100mA, …"),
			new Attribute(7, 0, 1, "OTS", "Over-temperature shutdown", "Clear, Raised"),
			new Attribute(7, 1, 1, "AOCP", "Channel A over-current shutdown", "Clear, Raised"),
			new Attribute(7, 2, 1, "BOCP", "Channel B over-current shutdown", "Clear, Raised"),
			new Attribute(7, 3, 1, "APDF", "Channel A pre-driver fault", "Clear, Raised"),
			new Attribute(7, 4, 1, "BPDF", "Channel B pre-driver fault", "Clear, Raised"),
			new Attribute(7, 5, 1, "UVLO", "Undervoltage lockout due to low VM", "Clear, Raised"),
			new Attribute(7, 6, 1, "STD", "Stall detected due to low back EMF", "Clear, Raised"),
			new Attribute(7, 7, 1, "STDLAT", "Latched stall detect", "Clear, Raised"),
			new Attribute(8, 0, 16, "pwmstart", "Start speed, longest pulse width", "0..64k"),
			new Attribute(9, 0, 16, "pwmtarget", "Target speed, shortest pulse width", "0..64k"),
			new Attribute(10, 0, 16, "pwmaccel", "Acceleration rate, pulse width decrement", "0..64k"),
			new Attribute(11, 0, 16, "pwmdecel", "Deceleration rate, pulse width increment", "0..64k"),

	};
}
