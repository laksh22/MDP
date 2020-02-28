package mdp14.mdp14app.model;

import java.io.PipedOutputStream;

public class IDblock {
	private String id = "";
	private Position position = null;
	//private Position position=new Position(1,10);

	public IDblock(String blockID, Position pos){
		this.position = pos;
		this.id = blockID;
	}

	public IDblock(String blockID, int posX, int posY){
		this.position = new Position(posX, posY);
		this.id = blockID;
	}

	public String getID() { return id; }
	public Position getPosition() {
		return position;
	}
	public void setPosition(Position position) {
		this.position = position;
	}

}
