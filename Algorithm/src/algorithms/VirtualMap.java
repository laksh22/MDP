package algorithms;

import simulator.arena.Arena;

public class VirtualMap {
	
	private boolean[][] _visited;
	private boolean[][] _cleared;
	private static VirtualMap _instance;
	
	private VirtualMap() {
		
		//testing - check result of exploration
/*		System.out.println("=========Explore Result===========");
		for (int a = Arena.MAP_WIDTH - 1; a >= 0; a--) {
			for (int b =0; b < Arena.MAP_LENGTH; b++) {
				System.out.print(mazeRef[b][a] + " ");
			}
			System.out.println();
		}
		System.out.println("==============END=================");*/
		
		_visited = new boolean[Arena.MAP_LENGTH][Arena.MAP_WIDTH];
		_cleared = new boolean[Arena.MAP_LENGTH][Arena.MAP_WIDTH];
	}

	public static VirtualMap getInstance() {
        if (_instance == null) {
            _instance = new VirtualMap();
        }
        return _instance;
    }
	
	
	public void updateVirtualMap(int[][] mazeRef) {

		for (int i = 0; i < Arena.MAP_LENGTH; i++) {
			for (int j = 0; j < Arena.MAP_WIDTH; j++) {
				_visited[i][j] = false;
				if (i == 0 || i == Arena.MAP_LENGTH - 1 || j == 0 || j == Arena.MAP_WIDTH - 1) {
					_cleared[i][j] = false;
				} else {
					_cleared[i][j] = true;
				}
			}
		}
		
		int i, j, u, v;
		
		for (i = 0; i < Arena.MAP_LENGTH ; i++) {
			for (j = 0; j < Arena.MAP_WIDTH ; j++) {
				if (mazeRef[i][j] != MazeExplorer.IS_EMPTY) {
					for (u = i - 1; u <= i + 1; u++) {
							for (v = j - 1; v <= j + 1; v++) {
								if (u >= 0 && v >= 0 && u < Arena.MAP_LENGTH && v < Arena.MAP_WIDTH) {
									_cleared[u][v] = false;
								}
							}
					} 
				}
			}
		}
	}

	public boolean[][] getCleared() {
		return _cleared;
	}

	public boolean checkIfVisited(int x, int y) {
		return _visited[x][y];
	}

	public void setVisited(int x, int y) {
		_visited[x][y] = true;
		
	}
	
	
	
	
	
}
