package algorithms;

import java.util.ArrayList;

public class Path {
	
	private ArrayList<Step> _steps = new ArrayList<Step>();
	
	public void prependStep(int index, int x, int y) {
		_steps.add(index, new Step(x, y));
	}
	
	public ArrayList<Step> getSteps() {
		return _steps;
	}
	
	public void combineSteps(ArrayList<Step> step) {
		_steps.remove(_steps.size()-1);
		_steps.addAll(step);
	}
	
	public int getStepSize() {
		return _steps.size();
	}
	
	public int getNumOfSteps() {
		return _steps.size();
	}
	
	public int[] getStep (int index) {
		int[] position = new int[2];
		position[0] = _steps.get(0).getX();
		position[1] = _steps.get(0).getY();
		return position;
	}
	
	public class Step {
		private int _x;
		private int _y;
		public Step(int x, int y) {
			_x = x;
			_y = y;
		}
		public int getX() {
			return _x;
		}
		public int getY() {
			return _y;
		}
	}
	
}
