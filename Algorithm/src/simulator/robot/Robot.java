package simulator.robot;

import java.io.IOException;

import datatypes.Message;
import datatypes.Orientation;
import main.RobotSystem;
import simulator.Controller;
import simulator.arena.Arena;
import tcpcomm.PCClient;

public class Robot {
	private static Robot _instance;
	private int _speed;
	private int _stepsSinceLastCalibration;

	
	private Robot() {
		_stepsSinceLastCalibration = 0;
	}
	
	public static Robot getInstance() {
		if (_instance == null) {
			_instance = new Robot();
		}
		return _instance;
	}
	
	public void setSpeed(int speed) {
		_speed = speed;
	}
	
	public int getStepsSinceLastCalibration(){
		return _stepsSinceLastCalibration;
	}
	
	public void resetStepsSinceLastCalibration(){
		_stepsSinceLastCalibration = 0;
	}
	
	public int senseFront(int[] sensorPosition, Orientation robotOrientation) {
		
	
		Arena arena = Arena.getInstance();
		int numOfClearGrids;
		Orientation sensorOri = robotOrientation;
		numOfClearGrids = arena.getNumOfClearGrids(sensorPosition, sensorOri);
		if (numOfClearGrids > Sensor.SHORT_RANGE) {
			numOfClearGrids = Sensor.SHORT_RANGE;
		}
		
		return numOfClearGrids;
	}
	
	public int senseSideFront (int[] sensorPosition, Orientation robotOrientation) {
		Arena arena = Arena.getInstance();
		int numOfClearGrids;
		Orientation sensorOri = robotOrientation;
		numOfClearGrids = arena.getNumOfClearGrids(sensorPosition, sensorOri);
		if (numOfClearGrids > Sensor.SHORT_RANGE) {
			numOfClearGrids = Sensor.SHORT_RANGE;
		}
		return numOfClearGrids;
	}
	
	
	public int senseLeft(int[] sensorPosition, Orientation robotOrientation) {
		Arena arena = Arena.getInstance();
		int numOfClearGrids;
		Orientation sensorOri;
		switch (robotOrientation) {
			case NORTH:
				sensorOri = Orientation.WEST;
				break;
			case SOUTH:
				sensorOri = Orientation.EAST;
				break;
			case EAST:
				sensorOri = Orientation.NORTH;
				break;
			case WEST:
				sensorOri = Orientation.SOUTH;
				break;
			default:
				sensorOri = null;
		}
		numOfClearGrids = arena.getNumOfClearGrids(sensorPosition, sensorOri);
		if (numOfClearGrids > Sensor.LONG_RANGE) {
			numOfClearGrids = Sensor.LONG_RANGE;
		}
		return numOfClearGrids;
	}
	
	public int senseRight(int[] sensorPosition, Orientation robotOrientation) {
		Arena arena = Arena.getInstance();
		int numOfClearGrids;
		Orientation sensorOri;
		switch (robotOrientation) {
			case NORTH:
				sensorOri = Orientation.EAST;
				break;
			case SOUTH:
				sensorOri = Orientation.WEST;
				break;
			case EAST:
				sensorOri = Orientation.SOUTH;
				break;
			case WEST:
				sensorOri = Orientation.NORTH;
				break;
			default:
				sensorOri = null;
		}
		numOfClearGrids = arena.getNumOfClearGrids(sensorPosition, sensorOri);
		if (numOfClearGrids > Sensor.SHORT_RANGE) {
			numOfClearGrids = Sensor.SHORT_RANGE;
		}
		return numOfClearGrids;
	}

	public void turnRight() {
		Controller controller = Controller.getInstance();
		PCClient pcClient = controller.getPCClient();
		if (!RobotSystem.isRealRun()) {
			int stepTime = 1000 / _speed;
			try {
				Thread.sleep(stepTime);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		} else {
			try {
				pcClient.sendMessage(Message.TURN_RIGHT + Message.SEPARATOR);
	
				String feedback = pcClient.readMessage();
				while (!feedback.equals(Message.DONE)) {
					feedback = pcClient.readMessage();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		controller.turnRobotRight();
	}

	public void moveForward() {
		Controller controller = Controller.getInstance();
		PCClient pcClient = controller.getPCClient();

		if (!RobotSystem.isRealRun()) {
			int stepTime = 1000 / _speed;
			try {
				Thread.sleep(stepTime);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		} else {
			try {
				pcClient.sendMessage(Message.MOVE_FORWARD + Message.SEPARATOR);
				String feedback = pcClient.readMessage();
				while (!feedback.equals(Message.DONE)) {
					feedback = pcClient.readMessage();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
			_stepsSinceLastCalibration ++;
		}
		controller.moveRobotForward();
	}

	public void turnLeft() {
		Controller controller = Controller.getInstance();
		PCClient pcClient = controller.getPCClient();
		if (!RobotSystem.isRealRun()) {
			int stepTime = 1000 / _speed;
			try {
				Thread.sleep(stepTime);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		} else {
			try {
				pcClient.sendMessage(Message.TURN_LEFT + Message.SEPARATOR);
				String feedback = pcClient.readMessage();
				while (!feedback.equals(Message.DONE)) {
					feedback = pcClient.readMessage();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		controller.turnRobotLeft();
	}

	public void moveForward(int count) {
		
		Controller controller = Controller.getInstance();
		PCClient pcClient = controller.getPCClient();
		
		if (!RobotSystem.isRealRun()) {
			int stepTime = 1000 / _speed;
			for (int i = 0; i < count; i++) {
				try {
					Thread.sleep(stepTime);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
		
				controller.moveRobotForward();
			}
		} else {
			try {
				if (count >= 10) {
					pcClient.sendMessage(Message.MOVE_FORWARD + count % 10 + count / 10 +  "|");
				} else {
					pcClient.sendMessage(Message.MOVE_FORWARD + count + Message.SEPARATOR);
				}

				String feedback = pcClient.readMessage();
				for (int i = 0; i < count; i++) {
					while (!feedback.equals(Message.DONE)) {
						feedback = pcClient.readMessage();
					}
					controller.moveRobotForward();
				}
				
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	public void calibrateRobotPosition() {
		Controller controller = Controller.getInstance();
		PCClient pcClient = controller.getPCClient();
		try {
			pcClient.sendMessage(Message.CALIBRATE + Message.SEPARATOR);
			String feedback = pcClient.readMessage();
			while (!feedback.equals(Message.DONE)) {
				feedback = pcClient.readMessage();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		
	}
	
	public Orientation calibrateAtStartZone(Orientation ori) {	
		switch (ori) {
			case NORTH:
				turnLeft();
				calibrateRobotPosition();
				turnLeft();
				calibrateRobotPosition();
				turnRight();
				turnRight();
				break;
			case SOUTH:
				calibrateRobotPosition();
				turnRight();
				calibrateRobotPosition();
				turnRight();			
				break;
			case EAST:
				turnRight();
				calibrateRobotPosition();
				turnRight();
				calibrateRobotPosition();
				turnRight();
				break;
			case WEST:
				calibrateRobotPosition();
				turnLeft();
				calibrateRobotPosition();
				turnRight();
				turnRight();
		}
		return Orientation.NORTH;
	}
}
