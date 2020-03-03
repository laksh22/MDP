package ntu.mdpg1app.model;

public class Position{
	int posX;
	int posY;
	
	public Position(int posX,int posY){
		this.posX=posX;
		this.posY=posY;
	}
	
	public int getPosY() {
		return posY;
	}
	public void setPosY(int posY) {
		this.posY = posY;
	}
	public int getPosX() {
		return posX;
	}
	public void setPosX(int posX) {
		this.posX = posX;
	}
	
	public int[][] getEdges(){
		int[][] edges = {{posX,posY},{posX+1,posY},{posX,posY+1},{posX+1,posY+1}};
		return edges;
	}

	
	public boolean equals(Position arg0) {
		if(posX==arg0.getPosX() && posY==arg0.getPosY()){
			return true;
		}
		return false;
	}
}
