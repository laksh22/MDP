package simulator.arena;

import java.awt.Color;

import javax.swing.JButton;

import datatypes.Orientation;



public class Arena {
	
	public static final int MAP_WIDTH = 20;
	public static final int MAP_LENGTH = 15;
	
	private static Arena _instance;
	private Boolean[][] _layout;
	
	private Arena() {
		
	}
	
	public static Arena getInstance() {
		if (_instance == null) {
			_instance = new Arena();
		}
		return _instance;
	}
	
	public Boolean[][] getLayout() {
		return _layout;
	}
	
	//When setting layout, it will take in the mapGrids array and invert the y axis. 
	//This is done to save the map in map descriptor much easily
	public void setLayout(JButton[][] mapGrids) {
		_layout = new Boolean[MAP_LENGTH][MAP_WIDTH];
		for (int x = 0; x < MAP_WIDTH; x++) {
			for (int y = 0; y < MAP_LENGTH; y++) {
				if (mapGrids[x][y].getBackground() == Color.RED) {
					_layout[y][19-x] = true;
				} else {
					_layout[y][19-x] = false;
				}
			}
		}
	}
	
	public int getNumOfClearGrids (int[] sensorPosition, Orientation sensorOrientation) {
		int numOfClearGrids = 0;
		switch (sensorOrientation) {
			case NORTH:
				for (int y = sensorPosition[1] + 1; y < Arena.MAP_WIDTH; y++) {
					if (_layout[sensorPosition[0]][y] == false) {
						numOfClearGrids++;
					} else {
						break;
					}
				}
				break;
			case SOUTH:
				for (int y = sensorPosition[1] - 1; y >= 0; y--) {
					if (_layout[sensorPosition[0]][y] == false) {
						numOfClearGrids++;
					} else {
						break;
					}
				}
				break;
			case EAST:
				for (int x = sensorPosition[0] + 1; x < Arena.MAP_LENGTH; x++) {
					if (_layout[x][sensorPosition[1]] == false) {
						numOfClearGrids++;
					} else {
						break;
					}
				}
				break;
			case WEST:
				for (int x = sensorPosition[0] - 1; x >= 0; x--) {
					if (_layout[x][sensorPosition[1]] == false) {
						numOfClearGrids++;
					} else {
						break;
					}
				}
		}
		return numOfClearGrids;
	}
}
