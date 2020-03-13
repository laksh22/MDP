package main;
import java.awt.EventQueue;


import simulator.Controller;


public class RobotSystem {
	
	private static boolean _isRealRun = true;

	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				new RobotSystem();
			}
		});
	}
	
	public RobotSystem() {
		Controller c = Controller.getInstance();
		c.run();
	}
	
	public static boolean isRealRun() {
		return _isRealRun;
	}
}
