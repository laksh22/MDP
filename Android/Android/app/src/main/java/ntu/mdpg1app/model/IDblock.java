package ntu.mdpg1app.model;

public class IDblock {
	private String id = "";
	private Position position = null;

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
