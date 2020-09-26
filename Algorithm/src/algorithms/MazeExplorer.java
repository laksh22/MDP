package algorithms;

import java.io.IOException;
import java.time.LocalTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.stream.Collectors;

import algorithms.Path.Step;

import java.util.HashMap; // import the HashMap class
import java.util.LinkedHashSet;

import datatypes.Message;
import datatypes.Movement;
import datatypes.Orientation;
import datatypes.SensorPosition;
import main.RobotSystem;
import simulator.Controller;
import simulator.arena.Arena;
import simulator.robot.Robot;
import simulator.robot.Sensor;
import tcpcomm.PCClient;

public class MazeExplorer {
	
	public static final int DETECT_1_GRID = 100000;
	public static final int DETECT_2_GRID = 10000;
	public static final int DETECT_3_GRID = 1000;
	public static final int DETECT_4_GRID = 100;
	public static final int DETECT_5_GRID = 10;
	public static final int DETECT_6_GRID = 1;
	
	public static final int RIGHT_CHECK = -2;
	public static final int UNEXPLORED = -1;
	public static final int IS_OBSTACLE = 1;
	public static final int IS_EMPTY = 0;
	private static final int RIGHT_NO_ACCESS = -1;
	private static final int RIGHT_UNSURE_ACCESS = -2;
	private static final int RIGHT_CAN_ACCESS = -3;
	public static final int[] GOAL = {13, 18};
//	public static final int[] GOAL = {8, 13};
	public static final int[] START = {1, 1};
	private static final int INVALID_SENSOR_VALUE = -1;
	private static final int CALIBRATION_THRESHOLD = 3;
	private static final int RIGHT_CALIBRATION_THRESHOLD = 2;
	private static MazeExplorer _instance;
	private Boolean[][] _isExplored;
	private int[][] _mazeRef, rightWallRef, imageRef, weightageRef;
	private Robot _robot;
	private int[] _robotPosition;
	private Orientation _robotOrientation;
	private boolean _hasExploredTillGoal;
	private boolean startImageRun = false, nextRun = false, isFastestPathBack= true, leftObstacleSelected = false;
	private Path _fastestPathBack = null; 
	private AStarPathFinder _pathFinder;
	private int leftTurnCounter = 0;
	private int leftCountdown = 2;
	private int[] lastCalibrate = new int[2];
	private boolean leftGotObstacle = false;
	
	private MazeExplorer() {
	}
	
	public void setLeftCoundown() {
		leftCountdown--;
	}
	
	public void setLeftCountdownBack() {
		leftCountdown = 2;
	}
	
	public boolean getleftGotObstacle() {
		return leftGotObstacle;
	}
	public void setleftGotObstacle() {
		leftGotObstacle = false;
	}
	public void setOrientation(Orientation orientation) {
		_robotOrientation = orientation;
	}
	
	public static MazeExplorer getInstance() {
	    if (_instance == null) {
	        _instance = new MazeExplorer();
	    }
	    return _instance;
	}

	public int[] getRobotPosition() {
		return _robotPosition;
	}
	
	private void setRobotPosition(int x, int y) {
		_robotPosition[0] = x;
		_robotPosition[1] = y;
	}
	
	public Orientation getRobotOrientation() {
		return _robotOrientation;
	}
	
	public int[][] getMazeRef() {
		return _mazeRef;
	}
	
	public int[][] getWeightageRef(){
		return weightageRef;
	}
	
	public void setMazeReObstacle(int x, int y) {
		_mazeRef[x][y] = IS_OBSTACLE;
	}
	
	public Boolean[][] getIsExplored() {
		return _isExplored;
	}
	
	
	public boolean areAllExplored() {
		for (int i = 0; i < Arena.MAP_LENGTH; i++) {
			for (int j = 0; j < Arena.MAP_WIDTH; j++) {
				if (_isExplored[i][j] == false) {
					return false;
				}
			}
		}
		return true;
	}
	
	public boolean hasExploredTillGoal() {
		return _hasExploredTillGoal;
	}
	
    public String getP1Descriptor() {
		String P1Binary = "11";
		int value;
		for (int j = 0; j < Arena.MAP_WIDTH; j++) {
			for (int i = 0; i < Arena.MAP_LENGTH; i++) {
				value = (_isExplored[i][j]) ? 1 : 0;
				P1Binary = P1Binary + value;
			}
		}
		P1Binary = P1Binary + "11";
		String temp, P1HexStr = "";
		int index = 0;
		
		while (index < P1Binary.length()) {
			temp = "";
			for (int i = 0; i < 4; i++) {
				temp = temp + P1Binary.charAt(index);
				index++;
			}
			P1HexStr = P1HexStr + Integer.toString(Integer.parseInt(temp, 2), 16);
		}
		return P1HexStr;
	}

	public String getP2Descriptor() {
		String P2Binary = "";
		for (int j = 0; j < Arena.MAP_WIDTH; j++) {
			for (int i = 0; i < Arena.MAP_LENGTH; i++) {
				if (_mazeRef[i][j] == IS_OBSTACLE) {
					P2Binary = P2Binary + 1;
				} else if (_mazeRef[i][j] == IS_EMPTY) {
					P2Binary = P2Binary + 0;
				}
			}
		}
		
		int remainder = P2Binary.length() % 8;
		
		for (int i = 0; i < 8 - remainder; i++) {
			P2Binary = P2Binary + "0";
		}
		
		String temp, P2HexStr = "";
		int index = 0;
		
		while (index < P2Binary.length()) {
			temp = "";
			for (int i = 0; i < 4; i++) {
				temp = temp + P2Binary.charAt(index);
				index++;
			}
			P2HexStr = P2HexStr + Integer.toString(Integer.parseInt(temp, 2), 16);
		}
		
		return P2HexStr;
	}
	
	public void explore(int[] robotPosition, Orientation robotOrientation) {
		
		
		Controller controller = Controller.getInstance();
		
		//if (!RobotSystem.isRealRun()) 
		init(robotPosition, robotOrientation);
		
		for (int i = robotPosition[0] - 1; i <= robotPosition[0] + 1; i++) {
			for (int j = robotPosition[1] - 1; j <= robotPosition[1] + 1; j++) {
				_isExplored[i][j] = true;
				_mazeRef[i][j] = IS_EMPTY;
				rightWallRef[i][j] = IS_EMPTY;
				imageRef[i][j] = IS_EMPTY;
				weightageRef[i][j] = IS_EMPTY;
			}
		}

		setIsExplored(_robotPosition, _robotOrientation, true);
		
		System.out.println("exploreAlongWall Goal Start");
		exploreAlongWall (GOAL);
		System.out.println("exploreAlongWall Goal End");
		
		if (!controller.hasReachedTimeThreshold()) {
			_hasExploredTillGoal = true;
			System.out.println("exploreAlongWall Start Start");
			exploreAlongWall (START);
			System.out.println("exploreAlongWall Start End");
		} else {
			_hasExploredTillGoal = false; //Timeout before reaching goal
		}
		
		hasNextRun();
		
		if(startImageRun)
			findImage();
		
		//Current robot position is not at start point, find fastest path back to start point
		System.out.println("After end");
		fastestPathBackToStart(); 
		
		if (RobotSystem.isRealRun()) {
			//Send Arduino the signal that exploration is done
			//_robotOrientation = _robot.calibrateAtStartZone(_robotOrientation);
			try {
				PCClient pcClient = PCClient.getInstance();
				pcClient.sendMessage(Message.EXPLORE_DONE + Message.SEPARATOR);
				String msgExDone = pcClient.readMessage();
				while (!msgExDone.equals(Message.DONE)) {
					msgExDone = pcClient.readMessage();
				}
				
				pcClient.sendMessageToAndroid(Message.EXPLORE_DONE);
				pcClient.sendMsgToRPI(Message.RPI_DONE);
				
			} catch (IOException e) {
				e.printStackTrace();
			}
		} else {
			adjustOrientationTo(Orientation.NORTH);
		}
	}
	
	private void fastestPathBackToStart() {
		System.out.println("Current pos: "+_robotPosition[0]+","+_robotPosition[1]);
		if (!isGoalPos(_robotPosition, START)) {
			System.out.println("Heading back to start point "+LocalTime.now());
			AStarPathFinder pathFinder = AStarPathFinder.getInstance();
			//Controller controller = Controller.getInstance();
			//Path backPath = controller.getFastestPathBack();
//			if(backPath != null) {
				Path backPath = pathFinder.findFastestPath(_robotPosition[0], _robotPosition[1], START[0], START[1], _mazeRef);
				_robotOrientation = pathFinder.moveRobotAlongFastestPath(backPath, _robotOrientation, false, false, false);
				_robotOrientation = _robot.calibrateAtStartZone(_robotOrientation);
				setRobotPosition(START[0], START[1]);
//			}
		} 
		else
			_robotOrientation = _robot.calibrateAtStartZone(_robotOrientation);
	}
	
	public void hasNextRun() {
		System.out.println("Start next Run");
		Controller controller = Controller.getInstance();
		boolean end = false;
		HashMap<Integer, int[]> positionHashMap = new HashMap<Integer, int[]>();
		HashMap<Integer, Integer> positionHashMap2 = new HashMap<Integer, Integer>();
		while (!controller.hasReachedTimeThreshold() && !areAllExplored() && !end) {
			nextRun = true;
			end = ExploreNextRound(_robotPosition, positionHashMap, positionHashMap2);
		}
	}
	
	public void updateImageRef() {
		for (int i = 0; i < Arena.MAP_LENGTH; i++) {
			for (int j = 0; j < Arena.MAP_WIDTH; j++) {
				if (_mazeRef[i][j] == IS_OBSTACLE && rightWallRef[i][j] != RIGHT_CHECK){
					imageRef[i][j] = IS_OBSTACLE;
					System.out.println("Image Ref: "+i+","+j);
				}		
			}
		}
	}
	
	private int[] getPos() {
		int[] obsPos = new int[2];
		
		for (int obsX = 0; obsX < Arena.MAP_LENGTH; obsX++) {
			for (int obsY = 0; obsY < Arena.MAP_WIDTH; obsY++) {
				if(imageRef[obsX][obsY] == IS_OBSTACLE) {
					obsPos[0] = obsX;
					obsPos[1] = obsY;
					return obsPos;
				}
			}
		}
		return null;
	}
	
	public void findImage() {
		
		updateImageRef();
		
		while(getPos()!=null) {
			VirtualMap virtualMap = VirtualMap.getInstance();
			AStarPathFinder pathFinder = AStarPathFinder.getInstance();
			Path fastestPath;
			Controller controller = Controller.getInstance();
			int[] nextRobotPosition = null, obsPos;
			virtualMap.updateVirtualMap(_mazeRef);
			
			obsPos = getPos();
			int obsX = obsPos[0];
			int obsY = obsPos[1];
			System.out.println("Next Point: "+obsX+","+obsY);
			System.out.println("Time Reach?: "+controller.hasReachedTimeThreshold());
			if (controller.hasReachedTimeThreshold()) {		
				System.out.println("Find Image: TIME OUT!");
				break;
			}
			if (imageRef[obsX][obsY] == IS_OBSTACLE){
				nextRobotPosition = getNearestRobotPositionTo(obsX, obsY, virtualMap, false);
				
				fastestPath = pathFinder.findFastestPath(_robotPosition[0], _robotPosition[1], nextRobotPosition[0], nextRobotPosition[1], _mazeRef);													
				
				_robotOrientation = pathFinder.moveRobotAlongFastestPath(fastestPath, _robotOrientation, true, true, true);
				virtualMap.updateVirtualMap(_mazeRef);
				
				if (_robotPosition[0] > obsX) {
					adjustOrientationTo(Orientation.WEST);
				} else if (_robotPosition[0] < obsX) {
					adjustOrientationTo(Orientation.EAST);
				} else if  (_robotPosition[1] > obsY) {
					adjustOrientationTo(Orientation.SOUTH);
				} else if  (_robotPosition[1] < obsY) {
					adjustOrientationTo(Orientation.NORTH);
				}
				int[] pos = new int[2];
				pos[0] = -1;
				pos[1] = -1;
				findImageAlongWall(pos, nextRobotPosition);
				//updateImageRef();
				//currentRobotPosition = nextRobotPosition;
			}
		}		
	}
	
	public void sendObsPosLeft(int[] robotPosition, Orientation ori, int x, int y, int blockAway) {
		Robot robot = Robot.getInstance();
		String msg = "";
		
		if(leftCountdown == 0 && leftGotObstacle == true) {
			robot.turnRight();
			robot.turnRight();
			
			msg = x+"_"+y+"_"+ori;
			System.out.println("Send to RPI Left Wall Image: "+msg);
			if (RobotSystem.isRealRun()) {
				Controller controller = Controller.getInstance();
				PCClient pcClient = controller.getPCClient();	
				
				try {
					Date startDate = new Date();
					Date endDate = new Date();
					int numSeconds = 0;
					pcClient.sendMsgToRPI(msg);
					System.out.println("Sent Take Picture Command");
					String feedback = pcClient.readMessage();
					while (!feedback.equals(Message.DONE)) {
						feedback = pcClient.readMessage();
						System.out.println("Reading Picture Taking Command");
						endDate = new Date();
						numSeconds = (int)((endDate.getTime() - startDate.getTime()) / 1000);
						System.out.println("Picture Taking Command Timing: "+numSeconds);
						if(((numSeconds % 2) == 0) || numSeconds < 10)
							pcClient.sendMsgToRPI(msg);
						else if(numSeconds >= 10)
							break;
						
					}
					System.out.println("Outside While Loop");
				} catch (IOException e) {
					e.printStackTrace();
				}
				System.out.println("Picture Command Completed");
			}
			robot.turnRight();
			robot.turnRight();
			leftCountdown = 3;
			leftGotObstacle = false;
		}

	}
	
	public void sendObstaclePos(int[] robotPosition, Orientation ori) throws IOException {
		String msg = "";
		

		if(ori == Orientation.NORTH && (robotPosition[0]+2) != 15)
			msg = (robotPosition[0]+2)+"_"+robotPosition[1]+"_"+ori;
		
		else if(ori == Orientation.SOUTH && (robotPosition[0]-2) != -1)
			msg = (robotPosition[0]-2)+"_"+robotPosition[1]+"_"+ori;
		
		else if(ori == Orientation.EAST && (robotPosition[1]-2) != -1)
			msg = (robotPosition[0])+"_"+(robotPosition[1]-2)+"_"+ori;
		
		else if(ori == Orientation.WEST && (robotPosition[1]+2) != 20)
			msg = (robotPosition[0])+"_"+(robotPosition[1]+2)+"_"+ori;
		
		System.out.println("Send to RPI: "+msg);
		if (RobotSystem.isRealRun()) {
			Controller controller = Controller.getInstance();
			PCClient pcClient = controller.getPCClient();	
			//pcClient.sendMsgToRPI(msg);
			
			try {
				Date startDate = new Date();
				Date endDate = new Date();
				int numSeconds = 0;
				pcClient.sendMsgToRPI(msg);
				System.out.println("Sent Take Picture Command");
				String feedback = pcClient.readMessage();
				while (!feedback.equals(Message.DONE)) {
					feedback = pcClient.readMessage();
					System.out.println("Reading Picture Taking Command");
					endDate = new Date();
					numSeconds = (int)((endDate.getTime() - startDate.getTime()) / 1000);
					System.out.println("Picture Taking Command Timing: "+numSeconds);
					if(((numSeconds % 2) == 0) && numSeconds < 10) {
						pcClient.sendMsgToRPI(msg);
					}
					else if(numSeconds >= 10)
						break;
					
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
			System.out.println("Picture Command Completed");
		}
		
	}

	
	//Start Exploring
	public void setIsExplored(int[] robotPosition, Orientation ori, boolean hasCalibration) {
		int count = 0;
		while(count == 0) {
			count++;
			String msgSensorValues = "";
			
			int[] leftObsPos = new int[2];
			
			if (RobotSystem.isRealRun()) {
				try {
					Controller controller = Controller.getInstance();
					PCClient pcClient = controller.getPCClient();
					
					pcClient.sendMessage(Message.READ_SENSOR_VALUES+Message.SEPARATOR);
					
					if(msgSensorValues.isEmpty()) {
						msgSensorValues = pcClient.readMessage();		
					}
					
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
			int[] frontSensorPosition = new int[2];
			int[] frontleftSensorPosition = new int[2];
			int[] frontrightSensorPosition = new int[2];
			int[] leftSensorPosition = new int[2];
			int[] rightSensorPosition = new int[2];
			int numOfClearGrids = 0;
			
			switch (ori) {
			case NORTH:
				frontSensorPosition[0] = robotPosition[0];
				frontSensorPosition[1] = robotPosition[1];
				frontleftSensorPosition[0] = robotPosition[0] - 1;
				frontleftSensorPosition[1] = robotPosition[1];
				frontrightSensorPosition[0] = robotPosition[0] + 1;
				frontrightSensorPosition[1] = robotPosition[1];
				leftSensorPosition[0] = robotPosition[0];
				leftSensorPosition[1] = robotPosition[1] + 1;
				rightSensorPosition[0] = robotPosition[0];
				rightSensorPosition[1] = robotPosition[1] + 1;
				
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseFront(frontSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.CF);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && numOfClearGrids + frontSensorPosition[1] < Arena.MAP_WIDTH) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontSensorPosition[0]][frontSensorPosition[1] + i] = true;
						_mazeRef[frontSensorPosition[0]][frontSensorPosition[1] + i] = IS_EMPTY;
						
						weightageRef[frontSensorPosition[0]][frontSensorPosition[1] + i] -= 1;
						//updateWeightageDecrease(i, frontSensorPosition[0], frontSensorPosition[1] + i);
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontSensorPosition[1] + numOfClearGrids + 1 < Arena.MAP_WIDTH) {
						_isExplored[frontSensorPosition[0]][frontSensorPosition[1] + numOfClearGrids + 1] = true;
						
						weightageRef[frontSensorPosition[0]][frontSensorPosition[1] + numOfClearGrids + 1] += 1;
						
						if((weightageRef[frontSensorPosition[0]][frontSensorPosition[1] + numOfClearGrids + 1]) > 0)
							_mazeRef[frontSensorPosition[0]][frontSensorPosition[1] + numOfClearGrids + 1] = IS_OBSTACLE;	
						else
							_mazeRef[frontSensorPosition[0]][frontSensorPosition[1] + numOfClearGrids + 1] = IS_EMPTY;	
						
						//updateWeightageIncrease(numOfClearGrids, frontSensorPosition[0], frontSensorPosition[1] + numOfClearGrids + 1);
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseSideFront(frontleftSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.LF);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && numOfClearGrids + frontleftSensorPosition[1] < Arena.MAP_WIDTH) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontleftSensorPosition[0]][frontleftSensorPosition[1] + i] = true;
						_mazeRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] + i] = IS_EMPTY;
						
						weightageRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] + i] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontleftSensorPosition[1] + numOfClearGrids + 1 < Arena.MAP_WIDTH) {
						_isExplored[frontleftSensorPosition[0]][frontleftSensorPosition[1] + numOfClearGrids + 1] = true;
						
						weightageRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] + numOfClearGrids + 1] += 1;
						
						if((weightageRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] + numOfClearGrids + 1]) > 0)
							_mazeRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] + numOfClearGrids + 1] = IS_OBSTACLE;
						else
							_mazeRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] + numOfClearGrids + 1] = IS_EMPTY;
						
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseSideFront(frontrightSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.RF);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && numOfClearGrids + frontrightSensorPosition[1] < Arena.MAP_WIDTH) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontrightSensorPosition[0]][frontrightSensorPosition[1] + i] = true;
						_mazeRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] + i] = IS_EMPTY;
						
						weightageRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] + i] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontrightSensorPosition[1] + numOfClearGrids + 1 < Arena.MAP_WIDTH) {
						_isExplored[frontrightSensorPosition[0]][frontrightSensorPosition[1] + numOfClearGrids + 1] = true;
						
						weightageRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] + numOfClearGrids + 1] += 1;
						
						if((weightageRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] + numOfClearGrids + 1]) > 0)		
							_mazeRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] + numOfClearGrids + 1] = IS_OBSTACLE;
						else
							_mazeRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] + numOfClearGrids + 1] = IS_EMPTY;
					
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseLeft(leftSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.L);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && leftSensorPosition[0] - numOfClearGrids >= 0) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[leftSensorPosition[0] - i][leftSensorPosition[1]] = true;
						_mazeRef[leftSensorPosition[0] - i][leftSensorPosition[1]] = IS_EMPTY;
						weightageRef[leftSensorPosition[0] - i][leftSensorPosition[1]] -= 1;
					}
					if (numOfClearGrids < Sensor.LONG_RANGE && leftSensorPosition[0] - numOfClearGrids - 1 >= 0) {
						_isExplored[leftSensorPosition[0] - numOfClearGrids - 1][leftSensorPosition[1]] = true;
						weightageRef[leftSensorPosition[0] - numOfClearGrids - 1][leftSensorPosition[1]] +=  1;
						
						if((weightageRef[leftSensorPosition[0] - numOfClearGrids - 1][leftSensorPosition[1]]) > 0) {
							_mazeRef[leftSensorPosition[0] - numOfClearGrids - 1][leftSensorPosition[1]] = IS_OBSTACLE;
							leftGotObstacle = true;
							leftObsPos[0] = leftSensorPosition[0] - numOfClearGrids - 1;
							leftObsPos[1] = leftSensorPosition[1];
						}
						else {
							_mazeRef[leftSensorPosition[0] - numOfClearGrids - 1][leftSensorPosition[1]] = IS_EMPTY;
							leftTurnCounter = 0;
						}
						
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseRight(rightSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.R);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && rightSensorPosition[0] + numOfClearGrids < Arena.MAP_LENGTH) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[rightSensorPosition[0] + i][rightSensorPosition[1]] = true;
						_mazeRef[rightSensorPosition[0] + i][rightSensorPosition[1]] = IS_EMPTY;
						weightageRef[rightSensorPosition[0] + i][rightSensorPosition[1]] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && rightSensorPosition[0] + numOfClearGrids + 1 < Arena.MAP_LENGTH) {
						_isExplored[rightSensorPosition[0] + numOfClearGrids + 1][rightSensorPosition[1]] = true;
						
						weightageRef[rightSensorPosition[0] + numOfClearGrids + 1][rightSensorPosition[1]] += 1;
						
						if((weightageRef[rightSensorPosition[0] + numOfClearGrids + 1][rightSensorPosition[1]]) > 0)
							_mazeRef[rightSensorPosition[0] + numOfClearGrids + 1][rightSensorPosition[1]] = IS_OBSTACLE;
						else
							_mazeRef[rightSensorPosition[0] + numOfClearGrids + 1][rightSensorPosition[1]] = IS_EMPTY;
						
					}
				}
				break;
				
			case SOUTH:
				frontSensorPosition[0] = robotPosition[0];
				frontSensorPosition[1] = robotPosition[1];
				frontleftSensorPosition[0] = robotPosition[0] + 1;
				frontleftSensorPosition[1] = robotPosition[1];
				frontrightSensorPosition[0] = robotPosition[0] - 1;
				frontrightSensorPosition[1] = robotPosition[1];
				leftSensorPosition[0] = robotPosition[0];
				leftSensorPosition[1] = robotPosition[1] - 1;
				rightSensorPosition[0] = robotPosition[0];
				rightSensorPosition[1] = robotPosition[1] - 1;
				
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseFront(frontSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.CF);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && frontSensorPosition[1] - numOfClearGrids >= 0) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontSensorPosition[0]][frontSensorPosition[1] - i] = true;
						_mazeRef[frontSensorPosition[0]][frontSensorPosition[1] - i] = IS_EMPTY;
						weightageRef[frontSensorPosition[0]][frontSensorPosition[1] - i] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontSensorPosition[1] - numOfClearGrids - 1 >= 0) {
						_isExplored[frontSensorPosition[0]][frontSensorPosition[1] - numOfClearGrids - 1] = true;
						
						weightageRef[frontSensorPosition[0]][frontSensorPosition[1] - numOfClearGrids - 1] += 1;
						
						if((weightageRef[frontSensorPosition[0]][frontSensorPosition[1] - numOfClearGrids - 1]) > 0)
							_mazeRef[frontSensorPosition[0]][frontSensorPosition[1] - numOfClearGrids - 1] = IS_OBSTACLE;
						else
							_mazeRef[frontSensorPosition[0]][frontSensorPosition[1] - numOfClearGrids - 1] = IS_EMPTY;
						
						
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseSideFront(frontleftSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.LF);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && frontleftSensorPosition[1] - numOfClearGrids >= 0) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontleftSensorPosition[0]][frontleftSensorPosition[1] - i] = true;
						_mazeRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] - i] = IS_EMPTY;
						weightageRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] - i] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontleftSensorPosition[1] - numOfClearGrids - 1 >= 0) {
						_isExplored[frontleftSensorPosition[0]][frontleftSensorPosition[1] - numOfClearGrids - 1] = true;
						
						weightageRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] - numOfClearGrids - 1] += 1;
						
						if((weightageRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] - numOfClearGrids - 1] > 0))
							_mazeRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] - numOfClearGrids - 1] = IS_OBSTACLE;
						else
							_mazeRef[frontleftSensorPosition[0]][frontleftSensorPosition[1] - numOfClearGrids - 1] = IS_EMPTY;
						
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseSideFront(frontrightSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.RF);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && frontrightSensorPosition[1] - numOfClearGrids >= 0) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontrightSensorPosition[0]][frontrightSensorPosition[1] - i] = true;
						_mazeRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] - i] = IS_EMPTY;
						weightageRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] - i] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontrightSensorPosition[1] - numOfClearGrids - 1 >= 0) {
						_isExplored[frontrightSensorPosition[0]][frontrightSensorPosition[1] - numOfClearGrids - 1] = true;
						
						weightageRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] - numOfClearGrids - 1] += 1;
						
						if((weightageRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] - numOfClearGrids - 1] > 0))
							_mazeRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] - numOfClearGrids - 1] = IS_OBSTACLE;
						else
							_mazeRef[frontrightSensorPosition[0]][frontrightSensorPosition[1] - numOfClearGrids - 1] = IS_EMPTY;
						
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseLeft(leftSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.L);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && leftSensorPosition[0] + numOfClearGrids < Arena.MAP_LENGTH) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[leftSensorPosition[0] + i][leftSensorPosition[1]] = true;
						_mazeRef[leftSensorPosition[0] + i][leftSensorPosition[1]] = IS_EMPTY;
						weightageRef[leftSensorPosition[0] + i][leftSensorPosition[1]] -= 1;
					}
					if (numOfClearGrids < Sensor.LONG_RANGE && leftSensorPosition[0] + numOfClearGrids + 1 < Arena.MAP_LENGTH) {
						_isExplored[leftSensorPosition[0] + numOfClearGrids + 1][leftSensorPosition[1]] = true;
						
						weightageRef[leftSensorPosition[0] + numOfClearGrids + 1][leftSensorPosition[1]] += 1;
						
						if((weightageRef[leftSensorPosition[0] + numOfClearGrids + 1][leftSensorPosition[1]] > 0)) {
							_mazeRef[leftSensorPosition[0] + numOfClearGrids + 1][leftSensorPosition[1]] = IS_OBSTACLE;
							leftGotObstacle = true;
							leftObsPos[0] = leftSensorPosition[0] + numOfClearGrids + 1;
							leftObsPos[1] = leftSensorPosition[1];
						}
						else {
							_mazeRef[leftSensorPosition[0] + numOfClearGrids + 1][leftSensorPosition[1]] = IS_EMPTY;
							leftTurnCounter = 0;
						}
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseRight(rightSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.R);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && rightSensorPosition[0] - numOfClearGrids >= 0) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[rightSensorPosition[0] - i][rightSensorPosition[1]] = true;
						_mazeRef[rightSensorPosition[0] - i][rightSensorPosition[1]] = IS_EMPTY;
						weightageRef[rightSensorPosition[0] - i][rightSensorPosition[1]] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && rightSensorPosition[0] - numOfClearGrids - 1 >= 0) {
						_isExplored[rightSensorPosition[0] - numOfClearGrids - 1][rightSensorPosition[1]] = true;
						
						weightageRef[rightSensorPosition[0] - numOfClearGrids - 1][rightSensorPosition[1]] += 1;
						
						if((weightageRef[rightSensorPosition[0] - numOfClearGrids - 1][rightSensorPosition[1]] > 0))
							_mazeRef[rightSensorPosition[0] - numOfClearGrids - 1][rightSensorPosition[1]] = IS_OBSTACLE;
						else
							_mazeRef[rightSensorPosition[0] - numOfClearGrids - 1][rightSensorPosition[1]] = IS_EMPTY;
						
					}
				}
				break;
			case EAST:
				frontSensorPosition[0] = robotPosition[0];
				frontSensorPosition[1] = robotPosition[1];
				frontleftSensorPosition[0] = robotPosition[0];
				frontleftSensorPosition[1] = robotPosition[1] + 1;
				frontrightSensorPosition[0] = robotPosition[0];
				frontrightSensorPosition[1] = robotPosition[1] - 1;
				leftSensorPosition[0] = robotPosition[0] + 1;
				leftSensorPosition[1] = robotPosition[1];
				rightSensorPosition[0] = robotPosition[0] + 1;
				rightSensorPosition[1] = robotPosition[1];
				
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseFront(frontSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.CF);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && frontSensorPosition[0] + numOfClearGrids < Arena.MAP_LENGTH) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontSensorPosition[0] + i][frontSensorPosition[1]] = true;
						_mazeRef[frontSensorPosition[0] + i][frontSensorPosition[1]] = IS_EMPTY;
						weightageRef[frontSensorPosition[0] + i][frontSensorPosition[1]] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontSensorPosition[0] + numOfClearGrids + 1 < Arena.MAP_LENGTH) {
						_isExplored[frontSensorPosition[0] + numOfClearGrids + 1][frontSensorPosition[1]] = true;
						
						weightageRef [frontSensorPosition[0] + numOfClearGrids + 1][frontSensorPosition[1]] += 1;
						
						if((weightageRef [frontSensorPosition[0] + numOfClearGrids + 1][frontSensorPosition[1]] > 0))
							_mazeRef [frontSensorPosition[0] + numOfClearGrids + 1][frontSensorPosition[1]] = IS_OBSTACLE;
						else
							_mazeRef [frontSensorPosition[0] + numOfClearGrids + 1][frontSensorPosition[1]] = IS_EMPTY;
						
						
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseSideFront(frontleftSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.LF);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && frontleftSensorPosition[0] + numOfClearGrids < Arena.MAP_LENGTH) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontleftSensorPosition[0] + i][frontleftSensorPosition[1]] = true;
						_mazeRef[frontleftSensorPosition[0] + i][frontleftSensorPosition[1]] = IS_EMPTY;
						weightageRef[frontleftSensorPosition[0] + i][frontleftSensorPosition[1]] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontleftSensorPosition[0] + numOfClearGrids + 1 < Arena.MAP_LENGTH) {
						_isExplored[frontleftSensorPosition[0] + numOfClearGrids + 1][frontleftSensorPosition[1]] = true;
						
						weightageRef [frontleftSensorPosition[0] + numOfClearGrids + 1][frontleftSensorPosition[1]] += 1;
						
						if((weightageRef [frontleftSensorPosition[0] + numOfClearGrids + 1][frontleftSensorPosition[1]] > 0))
							_mazeRef [frontleftSensorPosition[0] + numOfClearGrids + 1][frontleftSensorPosition[1]] = IS_OBSTACLE;
						else
							_mazeRef [frontleftSensorPosition[0] + numOfClearGrids + 1][frontleftSensorPosition[1]] = IS_EMPTY;
						
						
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseSideFront(frontrightSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.RF);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && frontrightSensorPosition[0] + numOfClearGrids < Arena.MAP_LENGTH) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontrightSensorPosition[0] + i][frontrightSensorPosition[1]] = true;
						_mazeRef [frontrightSensorPosition[0] + i][frontrightSensorPosition[1]]= IS_EMPTY;
						weightageRef [frontrightSensorPosition[0] + i][frontrightSensorPosition[1]] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontrightSensorPosition[0] + numOfClearGrids + 1 < Arena.MAP_LENGTH) {
						_isExplored[frontrightSensorPosition[0] + numOfClearGrids + 1][frontrightSensorPosition[1]] = true;
						
						weightageRef [frontrightSensorPosition[0] + numOfClearGrids + 1][frontrightSensorPosition[1]] += 1;
						if((weightageRef [frontrightSensorPosition[0] + numOfClearGrids + 1][frontrightSensorPosition[1]] > 0))
							_mazeRef [frontrightSensorPosition[0] + numOfClearGrids + 1][frontrightSensorPosition[1]] = IS_OBSTACLE;
						else
							_mazeRef [frontrightSensorPosition[0] + numOfClearGrids + 1][frontrightSensorPosition[1]] = IS_EMPTY;
						
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseLeft(leftSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.L);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && leftSensorPosition[1] + numOfClearGrids < Arena.MAP_WIDTH) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[leftSensorPosition[0]][leftSensorPosition[1] + i] = true;
						_mazeRef [leftSensorPosition[0]][leftSensorPosition[1] + i]= IS_EMPTY;
						weightageRef [leftSensorPosition[0]][leftSensorPosition[1] + i] -= 1;
					}
					if (numOfClearGrids < Sensor.LONG_RANGE && leftSensorPosition[1] + numOfClearGrids + 1 < Arena.MAP_WIDTH) {
						_isExplored[leftSensorPosition[0]][leftSensorPosition[1] + numOfClearGrids + 1] = true;
						weightageRef[leftSensorPosition[0]][leftSensorPosition[1] + numOfClearGrids + 1] += 1;
						
						if((weightageRef [leftSensorPosition[0]][leftSensorPosition[1] + numOfClearGrids + 1] > 0)) {
							_mazeRef[leftSensorPosition[0]][leftSensorPosition[1] + numOfClearGrids + 1] = IS_OBSTACLE;
							leftGotObstacle = true;
							leftObsPos[0] = leftSensorPosition[0];
							leftObsPos[1] = leftSensorPosition[1] + numOfClearGrids + 1;
						}
						else {
							_mazeRef[leftSensorPosition[0]][leftSensorPosition[1] + numOfClearGrids + 1] = IS_EMPTY;
							leftTurnCounter = 0;
						}	
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseRight(rightSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.R);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && rightSensorPosition[1] - numOfClearGrids >= 0) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[rightSensorPosition[0]][rightSensorPosition[1] - i] = true;
						_mazeRef[rightSensorPosition[0]][rightSensorPosition[1] - i]= IS_EMPTY;
						weightageRef[rightSensorPosition[0]][rightSensorPosition[1] - i] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && rightSensorPosition[1] - numOfClearGrids - 1 >= 0) {
						_isExplored[rightSensorPosition[0]][rightSensorPosition[1] - numOfClearGrids - 1] = true;
						
						weightageRef[rightSensorPosition[0]][rightSensorPosition[1] - numOfClearGrids - 1] += 1;
						
						if((weightageRef[rightSensorPosition[0]][rightSensorPosition[1] - numOfClearGrids - 1] > 0 )) 
							_mazeRef[rightSensorPosition[0]][rightSensorPosition[1] - numOfClearGrids - 1] = IS_OBSTACLE;
						else
							_mazeRef[rightSensorPosition[0]][rightSensorPosition[1] - numOfClearGrids - 1] = IS_EMPTY;
						
					}
				}
				break;
			case WEST:
				frontSensorPosition[0] = robotPosition[0];
				frontSensorPosition[1] = robotPosition[1];
				frontleftSensorPosition[0] = robotPosition[0];
				frontleftSensorPosition[1] = robotPosition[1] - 1;
				frontrightSensorPosition[0] = robotPosition[0];
				frontrightSensorPosition[1] = robotPosition[1] + 1;
				leftSensorPosition[0] = robotPosition[0] - 1;
				leftSensorPosition[1] = robotPosition[1];
				rightSensorPosition[0] = robotPosition[0] - 1;
				rightSensorPosition[1] = robotPosition[1];
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseFront(frontSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.CF);
				}
				
				if (numOfClearGrids != INVALID_SENSOR_VALUE && frontSensorPosition[0] - numOfClearGrids >= 0) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontSensorPosition[0] - i][frontSensorPosition[1]] = true;		
						_mazeRef[frontSensorPosition[0] - i][frontSensorPosition[1]]= IS_EMPTY;
						weightageRef[frontSensorPosition[0] - i][frontSensorPosition[1]]-= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontSensorPosition[0] - numOfClearGrids - 1 >= 0) {
						_isExplored[frontSensorPosition[0] - numOfClearGrids - 1][frontSensorPosition[1]] = true;
						weightageRef[frontSensorPosition[0] - numOfClearGrids - 1][frontSensorPosition[1]]+= 1;
						
						if((weightageRef[frontSensorPosition[0] - numOfClearGrids - 1][frontSensorPosition[1]] > 0))
							_mazeRef[frontSensorPosition[0] - numOfClearGrids - 1][frontSensorPosition[1]]= IS_OBSTACLE;
						else
							_mazeRef[frontSensorPosition[0] - numOfClearGrids - 1][frontSensorPosition[1]]= IS_EMPTY;
						
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseSideFront(frontleftSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.LF);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && frontleftSensorPosition[0] - numOfClearGrids >= 0) {
				for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontleftSensorPosition[0] - i][frontleftSensorPosition[1]] = true;
						_mazeRef[frontleftSensorPosition[0] - i][frontleftSensorPosition[1]]= IS_EMPTY;
						weightageRef[frontleftSensorPosition[0] - i][frontleftSensorPosition[1]] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontleftSensorPosition[0] - numOfClearGrids - 1 >= 0) {
						_isExplored[frontleftSensorPosition[0] - numOfClearGrids - 1][frontleftSensorPosition[1]] = true;
						weightageRef[frontleftSensorPosition[0] - numOfClearGrids - 1][frontleftSensorPosition[1]] += 1;
						
						if((weightageRef[frontleftSensorPosition[0] - numOfClearGrids - 1][frontleftSensorPosition[1]] > 0))
							_mazeRef[frontleftSensorPosition[0] - numOfClearGrids - 1][frontleftSensorPosition[1]]= IS_OBSTACLE;
						else
							_mazeRef[frontleftSensorPosition[0] - numOfClearGrids - 1][frontleftSensorPosition[1]]= IS_EMPTY;
						
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseSideFront(frontrightSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.RF);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && frontrightSensorPosition[0] - numOfClearGrids >= 0) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[frontrightSensorPosition[0] - i][frontrightSensorPosition[1]] = true;
						_mazeRef[frontrightSensorPosition[0] - i][frontrightSensorPosition[1]]= IS_EMPTY;
						weightageRef[frontrightSensorPosition[0] - i][frontrightSensorPosition[1]] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && frontrightSensorPosition[0] - numOfClearGrids - 1 >= 0) {
						_isExplored[frontrightSensorPosition[0] - numOfClearGrids - 1][frontrightSensorPosition[1]] = true;
						
						weightageRef[frontrightSensorPosition[0] - numOfClearGrids - 1][frontrightSensorPosition[1]] += 1;
						
						if((weightageRef[frontrightSensorPosition[0] - numOfClearGrids - 1][frontrightSensorPosition[1]] > 0))
							_mazeRef[frontrightSensorPosition[0] - numOfClearGrids - 1][frontrightSensorPosition[1]]= IS_OBSTACLE;
						else
							_mazeRef[frontrightSensorPosition[0] - numOfClearGrids - 1][frontrightSensorPosition[1]]= IS_EMPTY;
						
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseLeft(leftSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.L);
				}
				if (numOfClearGrids != INVALID_SENSOR_VALUE && leftSensorPosition[1] - numOfClearGrids >= 0) {
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[leftSensorPosition[0]][leftSensorPosition[1] - i] = true;
						_mazeRef[leftSensorPosition[0]][leftSensorPosition[1] - i] = IS_EMPTY;
						weightageRef[leftSensorPosition[0]][leftSensorPosition[1] - i] -= 1;
					}
					if (numOfClearGrids < Sensor.LONG_RANGE && leftSensorPosition[1] - numOfClearGrids - 1 >= 0) {
						_isExplored[leftSensorPosition[0]][leftSensorPosition[1] - numOfClearGrids - 1] = true;
						
						weightageRef[leftSensorPosition[0]][leftSensorPosition[1] - numOfClearGrids - 1] += 1;
						
						if((weightageRef[leftSensorPosition[0]][leftSensorPosition[1] - numOfClearGrids - 1] > 0)) {
							_mazeRef[leftSensorPosition[0]][leftSensorPosition[1] - numOfClearGrids - 1]= IS_OBSTACLE;
							leftGotObstacle = true;
							leftObsPos[0] = leftSensorPosition[0];
							leftObsPos[1] = leftSensorPosition[1] - numOfClearGrids - 1;
						}
						else {
							_mazeRef[leftSensorPosition[0]][leftSensorPosition[1] - numOfClearGrids - 1]= IS_EMPTY;
							leftTurnCounter = 0;
						}
					}
				}
				if (!RobotSystem.isRealRun()) {
					numOfClearGrids = _robot.senseRight(rightSensorPosition, ori);
				} else {
					numOfClearGrids = parseSensorValue(msgSensorValues, SensorPosition.R);
				}
				
				if (numOfClearGrids != INVALID_SENSOR_VALUE && rightSensorPosition[1] + numOfClearGrids < Arena.MAP_WIDTH) {
					
					for (int i = 2; i <= numOfClearGrids; i++) {
						_isExplored[rightSensorPosition[0]][rightSensorPosition[1] + i] = true;
						_mazeRef[rightSensorPosition[0]][rightSensorPosition[1] + i]= IS_EMPTY;
						weightageRef[rightSensorPosition[0]][rightSensorPosition[1] + i] -= 1;
					}
					if (numOfClearGrids < Sensor.SHORT_RANGE && rightSensorPosition[1] + numOfClearGrids + 1 < Arena.MAP_WIDTH) {
						
						_isExplored[rightSensorPosition[0]][rightSensorPosition[1] + numOfClearGrids + 1] = true;
						
						weightageRef[rightSensorPosition[0]][rightSensorPosition[1] + numOfClearGrids + 1] += 1;
						
						if((weightageRef[rightSensorPosition[0]][rightSensorPosition[1] + numOfClearGrids + 1] > 0))
							_mazeRef[rightSensorPosition[0]][rightSensorPosition[1] + numOfClearGrids + 1]= IS_OBSTACLE;
						else
							_mazeRef[rightSensorPosition[0]][rightSensorPosition[1] + numOfClearGrids + 1]= IS_EMPTY;
					}
				}
			}			
			
			Controller controller = Controller.getInstance();
			controller.updateMazeColor();
			
			if(leftObstacleSelected) {
				sendObsPosLeft(robotPosition, ori, leftObsPos[0], leftObsPos[1], numOfClearGrids);
			}
			
			//System.out.println("Deadend: "+isDeadEnd(controller.getPosition(), controller.getOrientation()));
			if (RobotSystem.isRealRun() && hasCalibration) {
//				Movement rightCali = canCalibrateRight(controller.getPosition(), controller.getOrientation());
//				if(rightCali == Movement.LR) {
//					_robot.calibrateRobotPosition(Movement.Z);
//				}
				
				if(!isGoalPos(_robotPosition, START)) {
					Movement mc = canCalibrateFront(controller.getPosition(), controller.getOrientation());
					//boolean deadend = false;
					if (mc == Movement.LR || mc == Movement.T || mc == Movement.Y ) {
						lastCalibrate[0] = robotPosition[0];
						lastCalibrate[1] = robotPosition[1];
						System.out.println("Calibrate Front");
						_robot.calibrateRobotPosition(mc);
						Movement mr = canCalibrateRight(controller.getPosition(), controller.getOrientation());
						Movement ml = canCalibrateLeft(controller.getPosition(), controller.getOrientation());
						if (mr != null) {
							//deadend = isDeadEnd(controller.getPosition(), controller.getOrientation());
							System.out.println("Calibrate Front + Right");
							_robot.turnRight();
							_robot.calibrateRobotPosition(mr);
//							if(deadend)
//								_robot.turnRight();
//							else
							_robot.turnLeft();
							//_robot.turn180();
							_robot.resetStepsSinceLastCalibration();
						}else if(ml != null){
							System.out.println("Calibrate Front + Left");
							_robot.turnLeft();
							_robot.calibrateRobotPosition(ml);
							_robot.turnRight();
							//_robot.turn180();
							_robot.resetStepsSinceLastCalibration();
						}
					} 
					else if(mc == Movement.R || mc == Movement.L || mc == Movement.M) {
						System.out.println("Calibrate Single");
						lastCalibrate[0] = robotPosition[0];
						lastCalibrate[1] = robotPosition[1];
						_robot.calibrateRobotPosition(mc);
						_robot.resetStepsSinceLastCalibration();
					}
					else {
						System.out.println("Calibrate Side");
						boolean needCalibration = _robot.getStepsSinceLastCalibration() > CALIBRATION_THRESHOLD;
						
						if (needCalibration && canCalibrateFront2(controller.getPosition(), controller.getOrientation()) != Movement.LR) {
							Movement mr = canCalibrateRight(controller.getPosition(), controller.getOrientation());
							Movement ml = canCalibrateLeft(controller.getPosition(), controller.getOrientation());
							if (mr != null) {
								System.out.println("Calibrate Side + Right");
								lastCalibrate[0] = robotPosition[0];
								lastCalibrate[1] = robotPosition[1];
								_robot.turnRight();
								if(mr == Movement.LR)
									_robot.calibrateRobotPosition();
								else
									_robot.calibrateRobotPosition(mr);
								_robot.turnLeft();
								_robot.resetStepsSinceLastCalibration();
							}else if(ml != null){
								System.out.println("Calibrate Side + Left");
								lastCalibrate[0] = robotPosition[0];
								lastCalibrate[1] = robotPosition[1];
								_robot.turnLeft();
								if(ml == Movement.LR)
									_robot.calibrateRobotPosition();
								else
									_robot.calibrateRobotPosition(ml);
								_robot.turnRight();
								_robot.resetStepsSinceLastCalibration();
							}
						}
					}
				}
			}
		}
	}
	


	public Orientation updateRobotOrientation (Movement move) {
		switch (move) {
			case TURN_LEFT:
				if (_robotOrientation == Orientation.NORTH) {
					_robotOrientation = Orientation.WEST;
				} else if (_robotOrientation == Orientation.SOUTH) {
					_robotOrientation = Orientation.EAST;
				} else if (_robotOrientation == Orientation.EAST) {
					_robotOrientation = Orientation.NORTH;
				} else {
					_robotOrientation = Orientation.SOUTH;
				}
				break;
			case TURN_RIGHT:
				if (_robotOrientation == Orientation.NORTH) {
					_robotOrientation = Orientation.EAST;
				} else if (_robotOrientation == Orientation.SOUTH) {
					_robotOrientation = Orientation.WEST;
				} else if (_robotOrientation == Orientation.EAST) {
					_robotOrientation = Orientation.SOUTH;
				} else {
					_robotOrientation = Orientation.NORTH;
				}
				break;
			case MOVE_FORWARD:
				break;
			case TURN_RIGHT_TWICE:
				
		}
		return _robotOrientation;
	}

	public int[] updateRobotPositionAfterMF(Orientation robotOrientation, int[] curRobotPosition) {
		switch (robotOrientation) {
			case NORTH:
				_robotPosition[0] = curRobotPosition[0];
				_robotPosition[1] = curRobotPosition[1] + 1;
				break;
			case SOUTH:
				_robotPosition[0] = curRobotPosition[0];
				_robotPosition[1] = curRobotPosition[1] - 1;
				break;
			case EAST:
				_robotPosition[0] = curRobotPosition[0] + 1;
				_robotPosition[1] = curRobotPosition[1];
				break;
			case WEST:
				_robotPosition[0] = curRobotPosition[0] - 1;
				_robotPosition[1] = curRobotPosition[1];
		}
		return _robotPosition;
	}

	public void adjustOrientationTo(Orientation ori) {
		switch (ori) {
			case NORTH:
				if (_robotOrientation == Orientation.SOUTH) {
					
					_robot.turnRight();
					_robotOrientation = Orientation.WEST;
					setIsExplored(_robotPosition, _robotOrientation, true);
					
					_robot.turnRight();
					_robotOrientation = Orientation.NORTH;
					setIsExplored(_robotPosition, _robotOrientation, true);
	
				} else if (_robotOrientation == Orientation.EAST) {
					
					_robot.turnLeft();
					_robotOrientation = Orientation.NORTH;
					setIsExplored(_robotPosition, _robotOrientation, true);
	
				} else if (_robotOrientation == Orientation.WEST) {
					
					_robot.turnRight();
					_robotOrientation = Orientation.NORTH;
					setIsExplored(_robotPosition, _robotOrientation, true);
					
				}
				break;
			case SOUTH:
				if (_robotOrientation == Orientation.NORTH) {
					
					_robot.turnRight();
					_robotOrientation = Orientation.EAST;
					setIsExplored(_robotPosition, _robotOrientation, true);
					
					_robot.turnRight();
					_robotOrientation = Orientation.SOUTH;
					setIsExplored(_robotPosition, _robotOrientation, true);
					
				} else if (_robotOrientation == Orientation.EAST) {
					
					_robot.turnRight();
					_robotOrientation = Orientation.SOUTH;
					setIsExplored(_robotPosition, _robotOrientation, true);
					
				} else if (_robotOrientation == Orientation.WEST) {
					_robot.turnLeft();
					_robotOrientation = Orientation.SOUTH;
					setIsExplored(_robotPosition, _robotOrientation, true);
				}
				break;
			case EAST:
				if (_robotOrientation == Orientation.NORTH) {
					_robot.turnRight();
					_robotOrientation = Orientation.EAST;
					setIsExplored(_robotPosition, _robotOrientation, true);
					
				} else if (_robotOrientation == Orientation.SOUTH) {
					
					_robot.turnLeft();
					_robotOrientation = Orientation.EAST;
					setIsExplored(_robotPosition, _robotOrientation, true);
					
				} else if (_robotOrientation == Orientation.WEST) {
		
					_robot.turnRight();
					_robotOrientation = Orientation.NORTH;
					setIsExplored(_robotPosition, _robotOrientation, true);
					
					_robot.turnRight();
					_robotOrientation = Orientation.EAST;
					setIsExplored(_robotPosition, _robotOrientation, true);
				}
				break;
			case WEST:
				if (_robotOrientation == Orientation.NORTH) {
					_robot.turnLeft();
					_robotOrientation = Orientation.WEST;
					setIsExplored(_robotPosition, _robotOrientation, true);
				} else if (_robotOrientation == Orientation.SOUTH) {
			
					_robot.turnRight();
					_robotOrientation = Orientation.WEST;
					setIsExplored(_robotPosition, _robotOrientation, true);
				} else if (_robotOrientation == Orientation.EAST) {
					
					_robot.turnRight();
					_robotOrientation = Orientation.SOUTH;
					setIsExplored(_robotPosition, _robotOrientation, true);
				
				
					_robot.turnRight();
					_robotOrientation = Orientation.SOUTH;
					setIsExplored(_robotPosition, _robotOrientation, true);
					
				}
		}
		
	}

	public void init(int[] robotPosition, Orientation robotOrientation) {
		_robot = Robot.getInstance();
		_robotPosition = new int[2];
		_robotPosition[0] = robotPosition[0];
		_robotPosition[1] = robotPosition[1];
		_robotOrientation = robotOrientation;
		_hasExploredTillGoal = false;
		_pathFinder  = AStarPathFinder.getInstance();
		lastCalibrate[0] = 1;
		lastCalibrate[1] = 1;
		_isExplored = new Boolean[Arena.MAP_LENGTH][Arena.MAP_WIDTH];
		_mazeRef = new int[Arena.MAP_LENGTH][Arena.MAP_WIDTH];
		rightWallRef = new int[Arena.MAP_LENGTH][Arena.MAP_WIDTH];
		imageRef = new int[Arena.MAP_LENGTH][Arena.MAP_WIDTH];
		weightageRef = new int[Arena.MAP_LENGTH][Arena.MAP_WIDTH];
		
		for (int i = 0; i < Arena.MAP_LENGTH; i++) {
			for (int j = 0; j < Arena.MAP_WIDTH; j++) {
				_isExplored[i][j] = false;
			}
		}
		for (int i = 0; i < Arena.MAP_LENGTH; i++) {
			for (int j = 0; j < Arena.MAP_WIDTH; j++) {
				_mazeRef[i][j] = UNEXPLORED;
				rightWallRef[i][j] = UNEXPLORED;
				imageRef[i][j] = UNEXPLORED;
				weightageRef[i][j] = 0;
			}
		}
	}

	private boolean ExploreNextRound(int[] currentRobotPosition, HashMap<Integer, int[]> positionHashMap,
										HashMap<Integer, Integer> positionHashMap2) {
		
		VirtualMap virtualMap = VirtualMap.getInstance();
		AStarPathFinder pathFinder = AStarPathFinder.getInstance();
		Path fastestPath;
		boolean isExhaustedViaFront = true, isExhaustedViaFrontside = true;
		boolean end;
		Controller controller = Controller.getInstance();
		int[] nextRobotPosition = null, checkExist , store = new int[2];
		virtualMap.updateVirtualMap(_mazeRef);

		for (int obsY = 0; obsY < Arena.MAP_WIDTH; obsY++) {
			for (int obsX = 0; obsX < Arena.MAP_LENGTH; obsX++) {
				if (controller.hasReachedTimeThreshold()) {		
					return true;
				}
				if (_mazeRef[obsX][obsY] == UNEXPLORED){
					nextRobotPosition = getNearestRobotPositionTo(obsX, obsY, virtualMap, false);
					if (nextRobotPosition != null) {
						isExhaustedViaFront = false;
					} else {
						//isExhaustedViaFront = true;
						continue;
					}
					
					fastestPath = pathFinder.findFastestPath(currentRobotPosition[0], currentRobotPosition[1], nextRobotPosition[0], nextRobotPosition[1], _mazeRef);													
					
					_robotOrientation = pathFinder.moveRobotAlongFastestPath(fastestPath, _robotOrientation, true, true, false);
					virtualMap.updateVirtualMap(_mazeRef);
					
					if (_robotPosition[0] > obsX) {
						adjustOrientationTo(Orientation.WEST);
					} else if (_robotPosition[0] < obsX) {
						adjustOrientationTo(Orientation.EAST);
					} else if  (_robotPosition[1] > obsY) {
						adjustOrientationTo(Orientation.SOUTH);
					} else if  (_robotPosition[1] < obsY) {
						adjustOrientationTo(Orientation.NORTH);
					}
					
					currentRobotPosition = nextRobotPosition;
					
					checkExist = new int[2];
					if(positionHashMap.containsKey(currentRobotPosition[0])) {
						store = positionHashMap.get(currentRobotPosition[0]);
						if(store[0] == currentRobotPosition[1]) {
							store[1]++;
							if(store[1]>2)
								return true;
							else {
								checkExist[0] = currentRobotPosition[1];
								checkExist[1] = store[1];
								positionHashMap.put(currentRobotPosition[0], checkExist);
							}
						}
					}
					else {
						checkExist[0] = currentRobotPosition[1];
						checkExist[1] = 1;
						positionHashMap.put(currentRobotPosition[0], checkExist);
					}
					
				}
			}
		}
		if (isExhaustedViaFront) {
			for (int obsY = 0; obsY < Arena.MAP_WIDTH; obsY++) {
				for (int obsX = 0; obsX < Arena.MAP_LENGTH; obsX++) {
					if (_mazeRef[obsX][obsY] == UNEXPLORED){
						
						nextRobotPosition = getNearestRobotPositionTo(obsX, obsY, virtualMap, true);
						if (nextRobotPosition != null) {
							isExhaustedViaFrontside = false;
						} else {
							continue;
						}
						
						
						fastestPath = pathFinder.findFastestPath(currentRobotPosition[0], currentRobotPosition[1], nextRobotPosition[0], nextRobotPosition[1], _mazeRef);
						
						_robotOrientation = pathFinder.moveRobotAlongFastestPath(fastestPath, _robotOrientation, true, true, false);
						virtualMap.updateVirtualMap(_mazeRef);
						
						if (_robotPosition[0] > obsX) {
							adjustOrientationTo(Orientation.WEST);
						} else if (_robotPosition[0] < obsX) {
							adjustOrientationTo(Orientation.EAST);
						} else if  (_robotPosition[1] > obsY) {
							adjustOrientationTo(Orientation.SOUTH);
						} else if  (_robotPosition[1] < obsY) {
							adjustOrientationTo(Orientation.NORTH);
						}
						currentRobotPosition = nextRobotPosition;	
						if(positionHashMap2.containsKey(currentRobotPosition[0])) {
							if(positionHashMap2.get(currentRobotPosition[0]) == currentRobotPosition[1]) {
								return true;
							}
						}
						positionHashMap2.put(currentRobotPosition[0], currentRobotPosition[1]);
					}
				}
			}
		}
		if (isExhaustedViaFront && isExhaustedViaFrontside) {
			fastestPath = pathFinder.findFastestPath(currentRobotPosition[0], currentRobotPosition[1], 
								MazeExplorer.START[0], MazeExplorer.START[1], _mazeRef);
			_robotOrientation = pathFinder.moveRobotAlongFastestPath(fastestPath, _robotOrientation, true, true, false);
			end = true;
		} else 
			end = false;
		
		return end;
	}

	private int[] getNearestRobotPositionTo (int obsX, int obsY, VirtualMap virtualMap, boolean isExhausted) {
		int nearestPosition[] = new int[2];
		boolean[][] cleared = virtualMap.getCleared();
		boolean isClearedAhead;
		for (int radius = 2; radius <= Sensor.LONG_RANGE; radius ++) {
			for (int y = 0; y <= obsY + radius; y++) {
				//for (int x = 0; x <= obsX + radius; x++) {
				for (int x = obsX + radius; x >= 0; x--) {
					if (x == obsX - radius || x == obsX + radius || y == obsY - radius || y == obsY + radius) {
						if (x >= 0 && y >= 0 && x < Arena.MAP_LENGTH && y < Arena.MAP_WIDTH) {
							if (cleared[x][y]) {
								if ((Math.abs(obsX + obsY - x - y) == radius) && (x == obsX || y == obsY)) {
									isClearedAhead = true;
									if (x > obsX) {
										for(int i = obsX + 1; i < x; i++) {
												if (_mazeRef[i][y] != IS_EMPTY) {
													isClearedAhead = false;
											}
										}
									} else if (x < obsX) {
										for(int i = x + 1; i < obsX; i++) {
											if (_mazeRef[i][y] != IS_EMPTY) {
												isClearedAhead = false;
											}
										}
									} else if (y > obsY) {
										for(int j = obsY + 1; j < y; j++) {
											if (_mazeRef[x][j] != IS_EMPTY) {
												isClearedAhead = false;
											}
										}
									} else if (y < obsY) {
										for(int j = y + 1; j < obsY; j++) {
											if (_mazeRef[x][j] != IS_EMPTY) {
												isClearedAhead = false;
											}
										}
									}
									if (isClearedAhead) {
										nearestPosition[0] = x;
										nearestPosition[1] = y;
										return nearestPosition;
									}
								}
							}
						}
					}
				}
			}
		}
		if (isExhausted) {
			for (int radius = 2; radius <= Sensor.SHORT_RANGE; radius ++) {
				for (int y = obsY - radius; y <= obsY + radius; y++) {
					for (int x = obsX - radius; x <= obsX + radius; x++) {
						if (x == obsX - radius || x == obsX + radius || y == obsY - radius || y == obsY + radius) {
							if (x >= 0 && y >= 0 && x < Arena.MAP_LENGTH && y < Arena.MAP_WIDTH) {
								if (cleared[x][y]) {
						
									if (Math.abs(x-obsX) + Math.abs(y-obsY) == radius + 1 ) {
								
										isClearedAhead = true;
										if (x == obsX + radius && y > obsY) {
											for(int i = obsX + 1; i < x; i++) {
													if (_mazeRef[i][obsY] != IS_EMPTY) {
														isClearedAhead = false;
												}
											}
										} else if(x == obsX + radius && y < obsY) {
											for(int i = obsX + 1; i < x; i++) {
												if (_mazeRef[i][obsY] != IS_EMPTY) {
													isClearedAhead = false;
											}
										}
										} else if (x == obsX - radius && y > obsY) {
											for(int i = x + 1; i < obsX; i++) {
												if (_mazeRef[i][obsY] != IS_EMPTY) {
													isClearedAhead = false;
												}
											}
										} else if (x == obsX - radius && y < obsY) {
											for(int i = x + 1; i < obsX; i++) {
												if (_mazeRef[i][obsY] != IS_EMPTY) {
													isClearedAhead = false;
												}
											}
										} else if (x < obsX && y == obsY + radius) {
											for(int j = obsY + 1; j < y; j++) {
												if (_mazeRef[obsX][j] != IS_EMPTY) {
													isClearedAhead = false;
												}
											}
										} else if (x > obsX && y == obsY + radius) {
											for(int j = obsY + 1; j < y; j++) {
												if (_mazeRef[obsX][j] != IS_EMPTY) {
													isClearedAhead = false;
												}
											}
										} else if (x < obsX && y == obsY - radius) {
											for(int j = y + 1; j < obsY; j++) {
												if (_mazeRef[obsX][j] != IS_EMPTY) {
													isClearedAhead = false;
												}
											}
										} else if (x > obsX && y == obsY - radius) {
											for(int j = y + 1; j < obsY; j++) {
												if (_mazeRef[obsX][j] != IS_EMPTY) {
													isClearedAhead = false;
												}
											}
										}
										if (isClearedAhead) {
											nearestPosition[0] = x;
											nearestPosition[1] = y;
									
											return nearestPosition;
										}
									
									}
								}
							}
						}
					}
				}
			}
		}
		return null;
	}
	
	private boolean checkGhostWall(int[] robotPosition, Orientation ori) {
		int x = robotPosition[0];
		int y = robotPosition[1];
		
		switch(ori) {
			case NORTH:
				if(_mazeRef[x+2][y+1] == IS_EMPTY && (x+2) != 15)
					return true;
				break;
			case SOUTH:
				if(_mazeRef[x-2][y-1] == IS_EMPTY && (x-2) != -1)
					return true;
				break;
			case EAST:
				if(_mazeRef[x+1][y-2] == IS_EMPTY && (y-2) != -1)
					return true;
				break;
			case WEST:
				if(_mazeRef[x-1][y+2] == IS_EMPTY && (y+2) != 20)
					return true;
				break;
		}
		return false;
	}
	
	private void exploreAlongWall (int[] goalPos) {
		Controller controller = Controller.getInstance();
		PCClient pc = PCClient.getInstance();
				
		while (!isGoalPos(_robotPosition, goalPos) && !controller.hasReachedTimeThreshold()) {
			int rightStatus = checkRightSide(_robotPosition, _robotOrientation);
			updateWall(_robotPosition, _robotOrientation);
			System.out.println("Right Status: "+rightStatus);
			if (rightStatus != RIGHT_NO_ACCESS) {
				System.out.println("Right Have Access");
				if (rightStatus == RIGHT_UNSURE_ACCESS) {
					System.out.println("Right Unsure Access");
					_robot.turnRight();
					updateRobotOrientation(Movement.TURN_RIGHT);
					setIsExplored(_robotPosition, _robotOrientation, true);

					if (hasAccessibleFront(_robotPosition, _robotOrientation)) {
						_robot.moveForward();
						updateRobotPositionAfterMF(_robotOrientation, _robotPosition);
						setIsExplored(_robotPosition, _robotOrientation, true);
					} else {
						_robot.turnLeft();
						updateRobotOrientation(Movement.TURN_LEFT);
					}
				} else { //rightStatus == RIGHT_CAN_ACCESS
					System.out.println("Right Sure Access");
						
					_robot.turnRight();
					updateRobotOrientation(Movement.TURN_RIGHT);
					setIsExplored(_robotPosition, _robotOrientation, true);
				
					_robot.moveForward();
					updateRobotPositionAfterMF(_robotOrientation, _robotPosition);
					setIsExplored(_robotPosition, _robotOrientation, true);
					
					if(checkGhostWall(_robotPosition, _robotOrientation)) {				
						AStarPathFinder pathFinder = AStarPathFinder.getInstance();
						Path backPath = pathFinder.findFastestPath(_robotPosition[0], _robotPosition[1], lastCalibrate[0], lastCalibrate[1], _mazeRef);
						_robotOrientation = pathFinder.moveRobotAlongFastestPath(backPath, _robotOrientation, false, false, false);
						_robotPosition[0] = lastCalibrate[0];
						_robotPosition[1] = lastCalibrate[1];
						
						_robot.turn180();
						switch(_robotOrientation) {
							case NORTH:
								_robotOrientation = Orientation.SOUTH;
								break;
							case SOUTH:
								_robotOrientation = Orientation.NORTH;
								break;
							case EAST:
								_robotOrientation = Orientation.WEST;
								break;
							case WEST:
								_robotOrientation = Orientation.EAST;
								break;
						}
					}	
				}
		
			} else if (hasAccessibleFront(_robotPosition, _robotOrientation)){ 
				System.out.println("Right No Access, Accessible Front");
				_robot.moveForward();
				updateRobotPositionAfterMF(_robotOrientation, _robotPosition);
				setIsExplored(_robotPosition, _robotOrientation, true);
			} else {
				System.out.println("Right No Access, No Accessible Front");
				_robot.turnLeft();
				updateRobotOrientation(Movement.TURN_LEFT);
				setIsExplored(_robotPosition, _robotOrientation, true);
			}				
			System.out.println("Explored all?: "+areAllExplored());
			if(areAllExplored() && isFastestPathBack) {
				_fastestPathBack = controller.getFastestPathBack();
				fastestPathBackToStart(); 
			}
		}
	}
	
	private void findImageAlongWall (int[] goalPos, int[] initial) {
		int move = 0;
		Controller controller = Controller.getInstance();
		PCClient pc = PCClient.getInstance();
		
		_robot.turnLeft();
		updateRobotOrientation(Movement.TURN_LEFT);
		
		while (!isGoalPos(_robotPosition, goalPos) && !controller.hasReachedTimeThreshold()) {
			int rightStatus = checkRightSide(_robotPosition, _robotOrientation);
			//System.out.println("Initial:"+initial[0]+","+initial[1]);
			eraseWall(_robotPosition, _robotOrientation);
			
			if (rightStatus != RIGHT_NO_ACCESS) {
				if (rightStatus == RIGHT_UNSURE_ACCESS) {
					_robot.turnRight();
					updateRobotOrientation(Movement.TURN_RIGHT);

					if (hasAccessibleFront(_robotPosition, _robotOrientation)) {
						_robot.moveForward();
						updateRobotPositionAfterMF(_robotOrientation, _robotPosition);
						move++;
						if(Arrays.equals(initial, _robotPosition) && move > 7) {
							return;
						}
						
					} else {
						_robot.turnLeft();
						updateRobotOrientation(Movement.TURN_LEFT);
					}
				} else { //rightStatus == RIGHT_CAN_ACCESS
					_robot.turnRight();
					updateRobotOrientation(Movement.TURN_RIGHT);
					
					_robot.moveForward();
					updateRobotPositionAfterMF(_robotOrientation, _robotPosition);
					move++;
					if(Arrays.equals(initial, _robotPosition) && move > 7) {
						return;
					}
				}
		
			} else if (hasAccessibleFront(_robotPosition, _robotOrientation)){ 
				_robot.moveForward();
				updateRobotPositionAfterMF(_robotOrientation, _robotPosition);
				move++;
				if(Arrays.equals(initial, _robotPosition) && move > 7) {
					return;
				}
			} else {
				_robot.turnLeft();
				updateRobotOrientation(Movement.TURN_LEFT);
			}
		}
	}
	
	//tells me if my curPos is equal to my goalPos
	private boolean isGoalPos (int[] curPos, int[] goalPos) {
		if (curPos[0] == goalPos[0] && curPos[1] == goalPos[1]) {
			return true;
		}
		return false;
	}
	
	private void updateWall(int[] curPos, Orientation ori) {
	    boolean hasObs = false;
	    for (int i=-1; i<2; i++) {
	      switch (ori) {
		      case NORTH:
		    	  if((curPos[0]+2) != 15 && _mazeRef[curPos[0]+2][curPos[1]+i] == IS_OBSTACLE) {
		    		  rightWallRef[curPos[0]+2][curPos[1]+i] = RIGHT_CHECK;
		    		  hasObs = true;
		    	  }
		    	  break;
		      case SOUTH:
		    	  if((curPos[0]-2) != -1 && _mazeRef[curPos[0]-2][curPos[1]+i] == IS_OBSTACLE) {
		    		  rightWallRef[curPos[0]-2][curPos[1]+i] = RIGHT_CHECK;
		    		  hasObs = true;
		    	  }
		    	  break;
		      case EAST:
		    	  if((curPos[1]-2) != -1 && _mazeRef[curPos[0]+i][curPos[1]-2] == IS_OBSTACLE) {
		    		  rightWallRef[curPos[0]+i][curPos[1]-2] = RIGHT_CHECK;
		    		  hasObs = true;
		    	  }
		    	  break;
		      case WEST:
		    	  if((curPos[1]+2) != 20 && _mazeRef[curPos[0]+i][curPos[1]+2] == IS_OBSTACLE) {
		    		  rightWallRef[curPos[0]+i][curPos[1]+2] = RIGHT_CHECK;
		    		  hasObs = true;
		    	  }
		    	  break;
	      }
	    }  
	    
      if(hasObs) {
	      try {
				sendObstaclePos(curPos, ori);
			} catch (IOException e1) {
				e1.printStackTrace();
			}
	      }
	  }
	
private void eraseWall(int[] curPos, Orientation ori) {
	
	boolean hasObs = false;
    for (int i=-1; i<2; i++) {
      switch (ori) {
	      case NORTH:
	    	  if((curPos[0]+2) != 15 && _mazeRef[curPos[0]+2][curPos[1]+i] == IS_OBSTACLE) {
	    		  imageRef[curPos[0]+2][curPos[1]+i] = IS_EMPTY;
	    		  hasObs = true;
	    	  }
	    	  break;
	      case SOUTH:
	    	  if((curPos[0]-2) != -1 && _mazeRef[curPos[0]-2][curPos[1]+i] == IS_OBSTACLE) {
	    		  imageRef[curPos[0]-2][curPos[1]+i] = IS_EMPTY;
	    		  hasObs = true;
	    	  }
	    	  break;
	      case EAST:
	    	  if((curPos[1]-2) != -1 && _mazeRef[curPos[0]+i][curPos[1]-2] == IS_OBSTACLE) {
	    		  imageRef[curPos[0]+i][curPos[1]-2] = IS_EMPTY;
	    		  hasObs = true;
	    	  }
	    	  break;
	      case WEST:
	    	  if((curPos[1]+2) != 20 && _mazeRef[curPos[0]+i][curPos[1]+2] == IS_OBSTACLE) {
	    		  imageRef[curPos[0]+i][curPos[1]+2] = IS_EMPTY;
	    		  hasObs = true;
	    	  }
	    	  break;
      }
	}
	 
    if(hasObs && startImageRun) {
	      try {
				sendObstaclePos(curPos, ori);
			} catch (IOException e1) {
				e1.printStackTrace();
			}
	      }
    
  }
	
	private int checkRightSide (int[] curPos, Orientation ori) {
		int[] rightPos = new int[2];
		boolean hasUnexplored = false;
		
		switch (ori) {
			case NORTH:
				rightPos[0] = curPos[0] + 1;
				rightPos[1] = curPos[1];
				if (rightPos[0] + 1 >= Arena.MAP_LENGTH) {
					return RIGHT_NO_ACCESS;
				}
				for (int j = rightPos[1] - 1; j <= rightPos[1] + 1; j++) {
					if (_mazeRef[rightPos[0] + 1][j] == IS_OBSTACLE) {
						return RIGHT_NO_ACCESS;
							
					} else if (_mazeRef[rightPos[0] + 1][j] == UNEXPLORED) {
						hasUnexplored = true;
					}
				}
				break;
			case SOUTH:
				rightPos[0] = curPos[0] - 1;
				rightPos[1] = curPos[1];
				if (rightPos[0] - 1 < 0) {
					return RIGHT_NO_ACCESS;
				}
				for (int j = rightPos[1] - 1; j <= rightPos[1] + 1; j++) {
					if (_mazeRef[rightPos[0] - 1][j] == IS_OBSTACLE) {
							return RIGHT_NO_ACCESS;
					} else if (_mazeRef[rightPos[0] - 1][j] == UNEXPLORED) {
						hasUnexplored = true;
					}
				}
				break;
			case EAST:
				rightPos[0] = curPos[0];
				rightPos[1] = curPos[1] - 1;
				if (rightPos[1] - 1 < 0) {
					return RIGHT_NO_ACCESS;
				}
				for (int j = rightPos[0] - 1; j <= rightPos[0] + 1; j++) {
					if (_mazeRef[j][rightPos[1] - 1] == IS_OBSTACLE) {
							return RIGHT_NO_ACCESS;
					} else if (_mazeRef[j][rightPos[1] - 1] == UNEXPLORED) {
						hasUnexplored = true;
					}
				}
				break;
			case WEST:
				rightPos[0] = curPos[0];
				rightPos[1] = curPos[1] + 1;
				if (rightPos[1] + 1 >= Arena.MAP_WIDTH) {
					return RIGHT_NO_ACCESS;
				}
				for (int j = rightPos[0] - 1; j <= rightPos[0] + 1; j++) {
					if (_mazeRef[j][rightPos[1] + 1] == IS_OBSTACLE) {
							return RIGHT_NO_ACCESS;
					} else if (_mazeRef[j][rightPos[1] + 1] == UNEXPLORED) {
						hasUnexplored = true;
					}
				}
		}
		
		if (hasUnexplored) {
			return RIGHT_UNSURE_ACCESS;
		} else {
			return RIGHT_CAN_ACCESS;
		}
	}
	
	private boolean hasAccessibleFront(int[] curPos, Orientation ori) {
		int[] frontPos = new int[2];

		switch (ori) {
			case NORTH:
				frontPos[0] = curPos[0];
				frontPos[1] = curPos[1] + 1;
				if (frontPos[1] + 1 >= Arena.MAP_WIDTH) {
					return false;
				}
				for (int i = frontPos[0] - 1; i <= frontPos[0] + 1; i++) {
					if (_mazeRef[i][frontPos[1] + 1] == IS_OBSTACLE) {
							return false;
					}
				}
				return true;
			case SOUTH:
				frontPos[0] = curPos[0];
				frontPos[1] = curPos[1] - 1;
				if (frontPos[1] - 1 < 0) {
					return false;
				}
				for (int i = frontPos[0] - 1; i <= frontPos[0] + 1; i++) {
					if (_mazeRef[i][frontPos[1] - 1] == IS_OBSTACLE) {
							return false;
					}
				}
				return true;
			case EAST:
				frontPos[0] = curPos[0] + 1;
				frontPos[1] = curPos[1];
				if (frontPos[0] + 1 >= Arena.MAP_LENGTH) {
					return false;
				}
				for (int i = frontPos[1] - 1; i <= frontPos[1] + 1; i++) {
					if (_mazeRef[frontPos[0] + 1][i] == IS_OBSTACLE) {
							return false;
					}
				}
				return true;
			case WEST:
				frontPos[0] = curPos[0] - 1;
				frontPos[1] = curPos[1];
				if (frontPos[0] - 1 < 0) {
					return false;
				}
				for (int i = frontPos[1] - 1; i <= frontPos[1] + 1; i++) {
					if (_mazeRef[frontPos[0] - 1][i] == IS_OBSTACLE) {
							return false;
					}
				}
				return true;
		}
		return false;
	}

	private boolean canCalibrateAhead(String msgSensorValues) {
		Robot robot = Robot.getInstance();
		if(robot.getStepsSinceLastCalibration() == 0)
			return false;
		else
			return msgSensorValues.matches(Message.CALIBRATE_PATTERN);
	}
	
	private boolean canCalibrateAheadV2(String msgSensorValues) {
		Robot robot = Robot.getInstance();
		if(robot.getStepsSinceLastCalibration() == 0)
			return false;
		else if (msgSensorValues.matches(Message.CALIBRATE_PATTERN_CENTRE))
			return true;
		return false;
	}
	
	private Movement canCalibrateFront(int[] robotPosition, Orientation ori) {
		
		Robot robot = Robot.getInstance();
		if(robot.getStepsSinceLastCalibration() == 0)
			return null;
		else {
		
			int x = robotPosition[0];
			int y = robotPosition[1];
			switch(ori) {
				case NORTH:
					if(checkObstacleFront(x-1, y+2, Orientation.NORTH) && checkObstacleFront(x+1, y+2, Orientation.NORTH))
						return Movement.LR;
					if (checkObstacleFront(x-1, y+2, Orientation.NORTH) && checkObstacleFront(x, y+2, Orientation.NORTH))
						return Movement.T;
					else if (checkObstacleFront(x, y+2, Orientation.NORTH) && checkObstacleFront(x+1, y+2, Orientation.NORTH) )
						return Movement.Y;
					if (checkObstacleFront(x, y+2, Orientation.NORTH))
						return Movement.M;
					else if (checkObstacleFront(x-1, y+2, Orientation.NORTH) )
						return Movement.L;
					else if (checkObstacleFront(x+1, y+2, Orientation.NORTH) )
						return Movement.R;
					break;
					
				case SOUTH:
					if(checkObstacleFront(x-1, y-2, Orientation.SOUTH) && checkObstacleFront(x+1, y-2, Orientation.SOUTH))
						return Movement.LR;
					else if (checkObstacleFront(x+1, y-2, Orientation.NORTH) && checkObstacleFront(x, y-2, Orientation.NORTH))
						return Movement.T;
					else if (checkObstacleFront(x, y-2, Orientation.NORTH) && checkObstacleFront(x-1, y-2, Orientation.NORTH) )
						return Movement.Y;
					if (checkObstacleFront(x, y-2, Orientation.SOUTH))
						return Movement.M;
					else if (checkObstacleFront(x+1, y-2, Orientation.SOUTH) )
						return Movement.L;
					else if (checkObstacleFront(x-1, y-2, Orientation.SOUTH) )
						return Movement.R;
					break;
					
				case EAST:
					if(checkObstacleFront(x+2, y+1, Orientation.EAST) && checkObstacleFront(x+2, y-1, Orientation.EAST))
						return Movement.LR;
					else if (checkObstacleFront(x+2, y+1, Orientation.NORTH) && checkObstacleFront(x+2, y, Orientation.NORTH))
						return Movement.T;
					else if (checkObstacleFront(x+2, y, Orientation.NORTH) && checkObstacleFront(x+2, y-1, Orientation.NORTH) )
						return Movement.Y;
					if (checkObstacleFront(x+2, y, Orientation.EAST))
						return Movement.M;
					else if (checkObstacleFront(x+2, y+1, Orientation.EAST) )
						return Movement.L;
					else if (checkObstacleFront(x+2, y-1, Orientation.EAST) )
						return Movement.R;
					break;
				case WEST:
					if(checkObstacleFront(x-2, y+1, Orientation.WEST) && checkObstacleFront(x-2, y-1, Orientation.WEST))
						return Movement.LR;
					else if (checkObstacleFront(x-2, y, Orientation.NORTH) && checkObstacleFront(x-2, y-1, Orientation.NORTH))
						return Movement.T;
					else if (checkObstacleFront(x-2, y, Orientation.NORTH) && checkObstacleFront(x-2, y+1, Orientation.NORTH) )
						return Movement.Y;
					if (checkObstacleFront(x-2, y, Orientation.WEST))
						return Movement.M;
					else if (checkObstacleFront(x-2, y-1, Orientation.WEST) )
						return Movement.L;
					else if (checkObstacleFront(x-2, y+1, Orientation.WEST) )
						return Movement.R;
					break;
			}
			
			return null;
		}
	}
	
private Movement canCalibrateFront2(int[] robotPosition, Orientation ori) {
		
		Robot robot = Robot.getInstance();
		if(robot.getStepsSinceLastCalibration() == 0)
			return null;
		else {
		
			int x = robotPosition[0];
			int y = robotPosition[1];
			switch(ori) {
				case NORTH:
					if(checkObstacleFront(x-1, y+3, Orientation.NORTH) && checkObstacleFront(x+1, y+3, Orientation.NORTH))
						return Movement.LR;
					else if (checkObstacleFront(x, y+3, Orientation.NORTH))
						return Movement.M;
					else if (checkObstacleFront(x-1, y+3, Orientation.NORTH) )
						return Movement.L;
					else if (checkObstacleFront(x+1, y+3, Orientation.NORTH) )
						return Movement.R;
					break;
					
				case SOUTH:
					if(checkObstacleFront(x-1, y-3, Orientation.SOUTH) && checkObstacleFront(x+1, y-3, Orientation.SOUTH))
						return Movement.LR;
					else if (checkObstacleFront(x, y-3, Orientation.SOUTH))
						return Movement.M;
					else if (checkObstacleFront(x+1, y-3, Orientation.SOUTH) )
						return Movement.L;
					else if (checkObstacleFront(x-1, y-3, Orientation.SOUTH) )
						return Movement.R;
					break;
					
				case EAST:
					if(checkObstacleFront(x+3, y+1, Orientation.EAST) && checkObstacleFront(x+3, y-1, Orientation.EAST))
						return Movement.LR;
					else if (checkObstacleFront(x+3, y, Orientation.EAST))
						return Movement.M;
					else if (checkObstacleFront(x+3, y+1, Orientation.EAST) )
						return Movement.L;
					else if (checkObstacleFront(x+3, y-1, Orientation.EAST) )
						return Movement.R;
					break;
				case WEST:
					if(checkObstacleFront(x-3, y+1, Orientation.WEST) && checkObstacleFront(x-3, y-1, Orientation.WEST))
						return Movement.LR;
					else if (checkObstacleFront(x-3, y, Orientation.WEST))
						return Movement.M;
					else if (checkObstacleFront(x-3, y-1, Orientation.WEST) )
						return Movement.L;
					else if (checkObstacleFront(x-3, y+1, Orientation.WEST) )
						return Movement.R;
					break;
			}
			
			return null;
		}
	}
	
	private Movement canCalibrateLeft(int[] robotPosition, Orientation ori) {
		int x = robotPosition[0];
		int y = robotPosition[1];
		switch(ori) {
			case NORTH:
				if(checkObstacleLeft(x-2, y+1, Orientation.NORTH) && checkObstacleLeft(x-2, y-1, Orientation.NORTH))
					return Movement.LR;
				else if (checkObstacleLeft(x-2, y, Orientation.NORTH) && checkObstacleLeft(x-2, y-1, Orientation.NORTH))
					return Movement.T;
				else if (checkObstacleLeft(x-2, y, Orientation.NORTH) && checkObstacleLeft(x-2, y+1, Orientation.NORTH) )
					return Movement.Y;
				else if (checkObstacleLeft(x-2, y, Orientation.NORTH))
					return Movement.M;
				else if (checkObstacleLeft(x-2, y-1, Orientation.NORTH) )
					return Movement.L;
				else if (checkObstacleLeft(x-2, y+1, Orientation.NORTH) )
					return Movement.R;
				break;
			case SOUTH:
				if(checkObstacleLeft(x+2, y+1, Orientation.SOUTH) && checkObstacleLeft(x+2, y-1, Orientation.SOUTH))
					return Movement.LR;
				else if (checkObstacleLeft(x+2, y+1, Orientation.SOUTH) && checkObstacleLeft(x+2, y, Orientation.SOUTH))
					return Movement.T;
				else if (checkObstacleLeft(x+2, y, Orientation.SOUTH) && checkObstacleLeft(x+2, y-1, Orientation.SOUTH) )
					return Movement.Y;
				else if (checkObstacleLeft(x+2, y, Orientation.SOUTH))
					return Movement.M;
				else if (checkObstacleLeft(x+2, y+1, Orientation.SOUTH) )
					return Movement.L;
				else if (checkObstacleLeft(x+2, y-1, Orientation.SOUTH) )
					return Movement.R;
				break;
			case EAST:
				if(checkObstacleLeft(x-1, y+2, Orientation.EAST) && checkObstacleLeft(x+1, y+2, Orientation.EAST))
					return Movement.LR;
				else if (checkObstacleLeft(x-1, y+2, Orientation.EAST) && checkObstacleLeft(x, y+2, Orientation.EAST))
					return Movement.T;
				else if (checkObstacleLeft(x, y+2, Orientation.EAST) && checkObstacleLeft(x+1, y+2, Orientation.EAST) )
					return Movement.Y;
				else if (checkObstacleLeft(x, y+2, Orientation.EAST))
					return Movement.M;
				else if (checkObstacleLeft(x-1, y+2, Orientation.EAST) )
					return Movement.L;
				else if (checkObstacleLeft(x+1, y+2, Orientation.EAST) )
					return Movement.R;
				break;
			case WEST:
				
				if(checkObstacleLeft(x-1, y-2, Orientation.WEST) && checkObstacleLeft(x+1, y-2, Orientation.WEST))
					return Movement.LR;
				else if (checkObstacleLeft(x+1, y-2, Orientation.WEST) && checkObstacleLeft(x, y-2, Orientation.WEST))
					return Movement.T;
				else if (checkObstacleLeft(x, y-2, Orientation.WEST) && checkObstacleLeft(x-1, y-2, Orientation.WEST) )
					return Movement.Y;
				else if (checkObstacleLeft(x, y-2, Orientation.WEST))
					return Movement.M;
				else if (checkObstacleLeft(x+1, y-2, Orientation.WEST) )
					return Movement.L;
				else if (checkObstacleLeft(x-1, y-2, Orientation.WEST) )
					return Movement.R;
				break;
		}
		
		return null;
	}
	
	private boolean isDeadEnd(int[] robotPosition, Orientation ori) {
		int x = robotPosition[0];
		int y = robotPosition[1];
		switch(ori) {
			case NORTH:
				if((checkObstacleFront(x-1,y+2,Orientation.NORTH) || checkObstacleFront(x,y+2,Orientation.NORTH) || checkObstacleFront(x+1,y+2,Orientation.NORTH)) &&
				(checkObstacleLeft(x-2,y+1,Orientation.NORTH) || checkObstacleLeft(x-2,y,Orientation.NORTH) || checkObstacleLeft(x-2,y-1,Orientation.NORTH)) &&
				(checkObstacle(x+2,y+1,Orientation.NORTH) || checkObstacle(x+2,y,Orientation.NORTH) || checkObstacle(x+2,y-1,Orientation.NORTH))) 
					return true;
				break;
			case SOUTH:
				if((checkObstacle(x-2,y+1,Orientation.SOUTH) || checkObstacle(x-2,y,Orientation.SOUTH) || checkObstacle(x-2,y-1,Orientation.SOUTH)) &&
				(checkObstacleLeft(x+2,y+1,Orientation.SOUTH) || checkObstacleLeft(x+2,y,Orientation.SOUTH) || checkObstacleLeft(x+2,y-1,Orientation.SOUTH)) &&
				(checkObstacleFront(x-1,y-2,Orientation.SOUTH) || checkObstacleFront(x,y-2,Orientation.SOUTH) || checkObstacleFront(x+1,y-2,Orientation.SOUTH)))
					return true;
				break;
			case EAST:
				if((checkObstacleLeft(x-1,y+2,Orientation.EAST) || checkObstacleLeft(x,y+2,Orientation.EAST) || checkObstacleLeft(x+1,y+2,Orientation.EAST)) &&
				(checkObstacleFront(x+2,y+1,Orientation.EAST) || checkObstacleFront(x+2,y,Orientation.EAST) || checkObstacleFront(x+2,y-1,Orientation.EAST)) &&
				(checkObstacle(x-1,y-2,Orientation.EAST) || checkObstacle(x,y-2,Orientation.EAST) || checkObstacle(x+1,y-2,Orientation.EAST)))
					return true;
				break;
			case WEST:
				if((checkObstacle(x-1,y+2,Orientation.WEST) || checkObstacle(x,y+2,Orientation.WEST) || checkObstacle(x+1,y+2,Orientation.WEST)) &&
				(checkObstacleFront(x-2,y+1,Orientation.WEST) || checkObstacleFront(x-2,y,Orientation.WEST) || checkObstacleFront(x-2,y-1,Orientation.WEST)) &&
				(checkObstacleLeft(x-1,y-2,Orientation.WEST) || checkObstacleLeft(x,y-2,Orientation.WEST) || checkObstacleLeft(x+1,y-2,Orientation.WEST)))
					return true;
				break;
		}
		
		return false;
	}
	
	private Movement canCalibrateRight(int[] robotPosition, Orientation ori) {
		int x = robotPosition[0];
		int y = robotPosition[1];
		switch(ori) {
			case NORTH:
				if(checkObstacle(x+2, y+1, Orientation.NORTH) && checkObstacle(x+2, y-1, Orientation.NORTH))
					return Movement.LR;
				else if (checkObstacle(x+2, y+1, Orientation.NORTH) && checkObstacle(x+2, y, Orientation.NORTH))
					return Movement.T;
				else if (checkObstacle(x+2, y, Orientation.NORTH) && checkObstacle(x+2, y-1, Orientation.NORTH) )
					return Movement.Y;
				else if (checkObstacle(x+2, y, Orientation.NORTH))
					return Movement.M;
				else if (checkObstacle(x+2, y+1, Orientation.NORTH) )
					return Movement.L;
				else if (checkObstacle(x+2, y-1, Orientation.NORTH) )
					return Movement.R;
				break;
			case SOUTH:
				if(checkObstacle(x-2, y+1, Orientation.SOUTH) && checkObstacle(x-2, y-1, Orientation.SOUTH))
					return Movement.LR;
				else if (checkObstacle(x-2, y, Orientation.SOUTH) && checkObstacle(x-2, y-1, Orientation.SOUTH))
					return Movement.T;
				else if (checkObstacle(x-2, y, Orientation.SOUTH) && checkObstacle(x-2, y+1, Orientation.SOUTH) )
					return Movement.Y;
				else if (checkObstacle(x-2, y, Orientation.SOUTH))
					return Movement.M;
				else if (checkObstacle(x-2, y-1, Orientation.SOUTH))
					return Movement.L;
				else if (checkObstacle(x-2, y+1, Orientation.SOUTH))
					return Movement.R;
				break;
			case EAST:
				if(checkObstacle(x-1, y-2, Orientation.EAST) && checkObstacle(x+1, y-2, Orientation.EAST)) 
					return Movement.LR;
				else if (checkObstacle(x+1, y-2, Orientation.EAST) && checkObstacle(x, y-2, Orientation.EAST))
					return Movement.T;
				else if (checkObstacle(x, y-2, Orientation.EAST) && checkObstacle(x-1, y-2, Orientation.EAST) )
					return Movement.Y;
				else if (checkObstacle(x, y-2, Orientation.EAST))
					return Movement.M;
				else if (checkObstacle(x+1, y-2, Orientation.EAST))
					return Movement.L;
				else if (checkObstacle(x-1, y-2, Orientation.EAST))
					return Movement.R;
				break;
			case WEST:
				if(checkObstacle(x-1, y+2, Orientation.WEST) && checkObstacle(x+1, y+2, Orientation.WEST))
					return Movement.LR;
				else if (checkObstacle(x-1, y+2, Orientation.WEST) && checkObstacle(x, y+2, Orientation.WEST))
					return Movement.T;
				else if (checkObstacle(x, y+2, Orientation.WEST) && checkObstacle(x+1, y+2, Orientation.WEST) )
					return Movement.Y;
				else if (checkObstacle(x, y+2, Orientation.WEST))
					return Movement.M;
				else if (checkObstacle(x-1, y+2, Orientation.WEST) )
					return Movement.L;
				else if (checkObstacle(x+1, y+2, Orientation.WEST) )
					return Movement.R;
				break;
		}
		
		return null;
	}
	
	private boolean checkObstacleFront(int x, int y, Orientation ori) {
		switch(ori) {
			case NORTH:
				if(y == 20 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
			case SOUTH:
				if(y == -1 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
			case EAST:
				if(x == 15 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
			case WEST:
				if(x == -1 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
			}
			return false;
	}
	
	private boolean checkObstacle(int x, int y, Orientation ori) {
		switch(ori) {
			case NORTH:
				if(x == 15 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
			case SOUTH:
				if(x == -1 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
			case EAST:
				if(y == -1 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
			case WEST:
				if(y == 20 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
			}
			return false;
	}
	
	private boolean checkObstacleLeft(int x, int y, Orientation ori) {
		switch(ori) {
			case NORTH:
				if(x == -1 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
			case SOUTH:
				if(x == 15 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
			case EAST:
				if(y == 20 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
			case WEST:
				if(y == -1 ||_mazeRef[x][y] == IS_OBSTACLE)
					return true;
				break;
		}
			return false;
	}
	
	private Movement canCalibrateAside (int[] robotPosition, Orientation ori) {

		boolean leftHasRef = true, rightHasRef = true;
		Movement move = null;
		switch(ori) {
			case NORTH:
				if (robotPosition[0] - 2 == -1) {
					move = Movement.TURN_LEFT;
				} else if (robotPosition[0] + 2 == Arena.MAP_LENGTH) {
					move = Movement.TURN_RIGHT;
				} else {
					for (int i = -1; i <= 1; i++) {
						if (_mazeRef[robotPosition[0] - 2][robotPosition[1] + i] != IS_OBSTACLE) {
							leftHasRef = false;
						}
						if (_mazeRef[robotPosition[0] + 2][robotPosition[1] + i] != IS_OBSTACLE) {
							rightHasRef = false;
						}
					}
					if (leftHasRef) {
						move = Movement.TURN_LEFT;
					} else if (rightHasRef) {
						move = Movement.TURN_RIGHT;
					}
				}
				break;
			case SOUTH:
				if (robotPosition[0] - 2 == -1) {
					move = Movement.TURN_RIGHT;
				} else if (robotPosition[0] + 2 == Arena.MAP_LENGTH) {
					move = Movement.TURN_LEFT;
				} else {
					for (int i = -1; i <= 1; i++) {
						if (_mazeRef[robotPosition[0] - 2][robotPosition[1] + i] != IS_OBSTACLE) {
							rightHasRef = false;
						}
						if (_mazeRef[robotPosition[0] + 2][robotPosition[1] + i] != IS_OBSTACLE) {
							leftHasRef = false;
						}
					}
					if (leftHasRef) {
						move = Movement.TURN_LEFT;
					} else if (rightHasRef) {
						move = Movement.TURN_RIGHT;
					}
				}
				break;
			case EAST:
				if (robotPosition[1] - 2 == -1) {
					move = Movement.TURN_RIGHT;
				} else if (robotPosition[1] + 2 == Arena.MAP_WIDTH) {
					move = Movement.TURN_LEFT;
				} else {
					for (int i = -1; i <= 1; i++) {
						if (_mazeRef[robotPosition[0] + i][robotPosition[1] - 2] != IS_OBSTACLE) {
							rightHasRef = false;
						}
						if (_mazeRef[robotPosition[0] + i][robotPosition[1] + 2] != IS_OBSTACLE) {
							leftHasRef = false;
						}
					}
					if (leftHasRef) {
						move = Movement.TURN_LEFT;
					} else if (rightHasRef) {
						move = Movement.TURN_RIGHT;
					}
				}
				break;
			case WEST:
				if (robotPosition[1] - 2 == -1) {
					move = Movement.TURN_LEFT;
				} else if (robotPosition[1] + 2 == Arena.MAP_WIDTH) {
					move = Movement.TURN_RIGHT;
				} else {
					for (int i = -1; i <= 1; i++) {
						if (_mazeRef[robotPosition[0] + i][robotPosition[1] - 2] != IS_OBSTACLE) {
							leftHasRef = false;
						}
						if (_mazeRef[robotPosition[0] + i][robotPosition[1] + 2] != IS_OBSTACLE) {
							rightHasRef = false;
						}
					}
					if (leftHasRef) {
						move = Movement.TURN_LEFT;
					} else if (rightHasRef) {
						move = Movement.TURN_RIGHT;
					}
				}
		}
		return move;
	}

	public static int parseSensorValue(String msgSensorValues, SensorPosition sensorPosition) {
		//String sensorValues = msgSensorValues.substring(0, msgSensorValues.length() - 1);
		List<String> valueList = Arrays.asList(msgSensorValues.split(":"));
		//Left Short/Left Long : Right Short : Front Short : Front Left Short : Front Right Short
		
		int reading = INVALID_SENSOR_VALUE;
		
		switch (sensorPosition) {
		case L:
			reading = Integer.parseInt(valueList.get(0));
			break;
			
		case R:
			reading = Integer.parseInt(valueList.get(1));
			break;
			
		case CF:
			reading = Integer.parseInt(valueList.get(2));
			break;
			
		case LF:
			reading = Integer.parseInt(valueList.get(3));
			break;
			
		case RF:
			reading = Integer.parseInt(valueList.get(4));
			break;
		}
		
		return reading;
	}	
	
}
