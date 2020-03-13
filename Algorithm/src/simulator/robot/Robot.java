package simulator.robot;

import java.io.IOException;

import algorithms.MazeExplorer;
import datatypes.Message;
import datatypes.Movement;
import datatypes.Orientation;
import main.RobotSystem;
import simulator.Controller;
import simulator.arena.Arena;
import tcpcomm.PCClient;

public class Robot {
	private static Robot _instance;
	private int _speed;
	private int _stepsSinceLastCalibration;
	private int _stepsSinceLastSideCalibration;
	
	private Robot() {
		_stepsSinceLastCalibration = 0;
		_stepsSinceLastSideCalibration = 0;
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
	
	public int getStepsSinceLastSideCalibration(){
		return _stepsSinceLastCalibration;
	}
	
	public void resetStepsSinceLastSideCalibration(){
		_stepsSinceLastCalibration = 0;
	}
	
	public void incrementSideCalibration() {
		_stepsSinceLastCalibration++;
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

	public void turn180() {
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
				pcClient.sendMessage(Message.oneEighty + Message.SEPARATOR);
	
				String feedback = pcClient.readMessage();
				while (!feedback.equals(Message.DONE)) {
					feedback = pcClient.readMessage();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		controller.turnRobotRight();
		controller.turnRobotRight();
		sendToAndroid();
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
		sendToAndroid();
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
			_stepsSinceLastCalibration++;
		}
		
		controller.moveRobotForward();
		sendToAndroid();
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
		sendToAndroid();
	}
	
	public void moveForward(int count, String msg) {
		
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
//			try {
//				if (count >= 10) {
//					pcClient.sendMessage(Message.MOVE_FORWARD + count % 10 + count / 10 +  "|");
//				} else {
//					pcClient.sendMessage(Message.MOVE_FORWARD + count + Message.SEPARATOR);
//				}
//				
//				String feedback = pcClient.readMessage();
//				for (int i = 0; i < count; i++) {
//					while (!feedback.equals(Message.DONE)) {
//						feedback = pcClient.readMessage();
//					}
//					controller.moveRobotForward();
//				}
//			} catch (IOException e) {
//				e.printStackTrace();
//			}
			
			try {
				
				for (int i = 0; i < count; i++) {
					controller.moveRobotForward();
				}
				
				String feedback = "";
				int c = 0;				
				pcClient.sendMessage(msg);
				while (c != count) {
					feedback = pcClient.readMessage();
					if(feedback.equals(Message.DONE))
						c++;
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
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
		} 
	}
	
	public void sendToAndroid() {
		Controller controller = Controller.getInstance();
		MazeExplorer explorer = MazeExplorer.getInstance();
		PCClient pcClient = controller.getPCClient();
		String P1Descriptor, P2Descriptor;
		String message = "";
		P1Descriptor = explorer.getP1Descriptor();
		P2Descriptor = explorer.getP2Descriptor();	
		message = "DATA:"+P1Descriptor+","+P2Descriptor+","+controller.getPosition()[0]+","+controller.getPosition()[1]+","+controller.getOrientation();
		if(RobotSystem.isRealRun()) {
			pcClient.sendMessageToAndroid(message);
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
	
	public void calibrateRobotPosition(Movement m) {
		Controller controller = Controller.getInstance();
		PCClient pcClient = controller.getPCClient();
		try {
			if(m.name().equals("LR"))
				pcClient.sendMessage(Message.CALIBRATE + Message.SEPARATOR);
			else
				pcClient.sendMessage(m.name() + Message.SEPARATOR);
			String feedback = pcClient.readMessage();
			while (!feedback.equals(Message.DONE)) {
				feedback = pcClient.readMessage();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void calibrateRobotPositionViaFront() {
		Controller controller = Controller.getInstance();
		PCClient pcClient = controller.getPCClient();
		try {
			pcClient.sendMessage(Message.CALIBRATEFRONT + Message.SEPARATOR);
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
				turnLeft();
				calibrateRobotPosition();
				turnRight();
				calibrateRobotPosition();
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
				turnLeft();
				calibrateRobotPosition();
				turnRight();
				calibrateRobotPosition();
				turnRight();
		}
		return Orientation.NORTH;
	}
}
