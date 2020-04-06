package ntu.mdpg1app.model;


import java.util.ArrayList;

public class Map {
	public static Map map=null;
	public static Map getInstance(){
		if(map==null){
			map= new Map();
		}
		return map;
	}
	
	
	private static Position[] START_POSITION = {new Position(0,0),new Position(1,0),new Position(2,0),new Position(0,1),new Position(1,1),new Position(2,1),new Position(0,2),new Position(1,2),new Position(2,2)};
	private static Position[] END_POSITION = {new Position(14,19),new Position(14,18),new Position(14,17),new Position(13,19),new Position(13,18),new Position(13,17),new Position(12,19),new Position(12,18),new Position(12,17)};


	private static int exploredTiles [][] = new int[20][15];
	private static int obstacles [][] = new int[20][15];
	private static String obstacleString = "";

	private ArrayList<IDblock> numberedBlocks = new ArrayList<IDblock>();

	public  int[][] getObstacles() {
		return obstacles;
	}
	public int[][] getExploredTiles() {
		return exploredTiles;
	}
	public ArrayList<IDblock> getNumberedBlocks() { return numberedBlocks; }

	public void addNumberedBlocks(IDblock block) {
		for (IDblock nb:numberedBlocks) {
			//override me
			if(nb.getPosition().equals(block.getPosition())){
				numberedBlocks.remove(nb);
				break;
			}
		}
		numberedBlocks.add(block);
	}

	//in hexdecimal, from map descriptor file to arrays
	public void setMap(String exploredTileHex, String obstacleHex, String exploredObstacleHex) {

		obstacleString = exploredObstacleHex;
		String exploredTileBinary = HexBin.hexToBin(exploredTileHex);
		exploredTileBinary = exploredTileBinary.substring(2, exploredTileBinary.length()-2);
		  
		String obstacleBinary = HexBin.hexToBin(obstacleHex);
		if(obstacleBinary.length()>0){
			obstacleBinary =obstacleBinary.substring(2, obstacleBinary.length()-2);
		}
		  
		String exploredObstacleBinary=null;
		if(exploredObstacleHex!=null){
			exploredObstacleBinary = HexBin.hexToBin(exploredObstacleHex);
		}
		  
		int obstacleIndex=0;
		for(int i =0;i<exploredTileBinary.length();i++){
			char exploreBit = exploredTileBinary.charAt(i);
			char obstacleBit = '0';
			if(obstacleBinary.length()>0){
				  obstacleBit = obstacleBinary.charAt(i);
			}
			int y = (i-(i%15))/15;
			int x = i%15;
			exploredTiles[y][x]=0;
			obstacles[y][x]=0;
			if(obstacleBit=='1'){
				obstacles[y][x]=1;
			}
			if(exploreBit=='1'){
				exploredTiles[y][x]=1;
				if(exploredObstacleBinary!=null){
					char exploredObstacleBit = exploredObstacleBinary.charAt(obstacleIndex);

					  if(exploredObstacleBit=='1'){
						  obstacles[y][x]=1;
					  }
				}
				obstacleIndex++;
			}
			  
			if((x==0&&y==0)||(x==1&&y==0)||(x==2&&y==0)||
				  (x==0&&y==1)||(x==1&&y==1)||(x==2&&y==1)||
				  (x==0&&y==2)||(x==1&&y==2)||(x==2&&y==2)){
				  obstacles[y][x]=0;
			}

			if((x==14&&y==19)||(x==13&&y==19)||(x==12&&y==19)||
				  (x==14&&y==18)||(x==13&&y==18)||(x==12&&y==18)||
				  (x==14&&y==17)||(x==13&&y==17)||(x==12&&y==17)){
				  obstacles[y][x]=0;
			}
		}
	}

	public void setMapJson(String obstacleHex){
		String exploredTileHex="FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";

		String exploredTileBinary = HexBin.hexToBin(exploredTileHex);
		exploredTileBinary =exploredTileBinary.substring(2, exploredTileBinary.length()-2);

		String obstacleBinary = HexBin.hexToBin(obstacleHex);

		for(int i =0;i<exploredTileBinary.length();i++){
			char exploreBit = exploredTileBinary.charAt(i);
			int y = (i-(i%15))/15;
			int x = i%15;
			exploredTiles[y][x]=0;
			if(exploreBit=='1'){
				exploredTiles[y][x]=1;
			}
		}

		for(int i =0;i<obstacleBinary.length();i++){
			char obstacleBit = '0';
			if(obstacleBinary.length()>0){
				obstacleBit = obstacleBinary.charAt(i);
			}
			int y = (i-(i%15))/15;
			int x = i%15;
			obstacles[19-y][x]=0;
			if(obstacleBit=='1'){
				obstacles[19-y][x]=1;
			}
		}
	}


	public String getBinaryExplored(){
		String binaryExplored="11";
		int exploredTile[][]=Map.getInstance().getExploredTiles();
		for(int y =0;y<20;y++){
			for(int x =0;x<15;x++){
				binaryExplored=binaryExplored+exploredTile[y][x];
			}
		}
		binaryExplored=binaryExplored+"11";
		return binaryExplored;
	}
	public String getBinaryExploredObstacle(){
		//String binaryExplored="11";
		String binaryExploredObstacle="";
		int exploredTile[][]=Map.getInstance().getExploredTiles();
		int obstacles[][]=Map.getInstance().getObstacles();
		for(int y =0;y<20;y++){
			for(int x =0;x<15;x++){
				if(exploredTile[y][x]==1){
					binaryExploredObstacle=binaryExploredObstacle+obstacles[y][x];
				}
			}
		}
		//binaryExplored=binaryExplored+"11";
		if(binaryExploredObstacle.length()%8!=0){
			for(int i =0;i<binaryExploredObstacle.length()%8;i++){
				binaryExploredObstacle=binaryExploredObstacle+"0";
			}
		}
		return binaryExploredObstacle;
	}

	public String getObstacleStringFromAlgo() {
		return obstacleString;
	}

}
