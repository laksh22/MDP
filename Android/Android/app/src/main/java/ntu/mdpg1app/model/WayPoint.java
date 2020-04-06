package ntu.mdpg1app.model;

public class WayPoint {
	public static WayPoint wp=null;
	private Position position=null;
	//private Position position=new Position(1,10);
	public static WayPoint getInstance(){
		if(wp==null){
			wp= new WayPoint();
		}
		return wp;
	}

	public Position getPosition() {
		return position;
	}
	public void setPosition(Position position) {
		this.position = position;
	}

}
