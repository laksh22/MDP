package simulator;

import java.awt.BorderLayout;
import java.awt.CardLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.nio.file.FileSystems;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;

import main.RobotSystem;
import simulator.arena.Arena;
import simulator.arena.FileReaderWriter;

import java.awt.Font;

import javax.swing.JTextField;

import java.awt.FlowLayout;

public class UI extends JFrame implements ActionListener {

	private static final String EXPLORE_PANEL = "Explore arena";
	private static final String FFP_PANEL = "Find fastest path";
	private static final long serialVersionUID = 1L;
	private JPanel _contentPane, _mapPane, _ctrlPane, _mazePane;
	private JLabel _status, _timer, _coverageUpdate;
	private JButton[][] _mapGrids, _mazeGrids;
	private Controller _controller;
	private JTextField[] _exploreTextFields, _ffpTextFields;
	private JButton _exploreBtn, _ffpBtn, _stopExploreBtn;

	/**
	 * Create the simulator.
	 */
	public UI() {
		super("MDP Simulator - Arena Exploration & Fastest Path Computation");
		setResizable(false);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		_contentPane = new JPanel();
		_contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		_contentPane.setLayout(new BorderLayout(0, 0));
		setContentPane(_contentPane);
		initContentPane(_contentPane);
		pack();
	}

	public void setExploreBtnEnabled(boolean value) {
		_exploreBtn.setEnabled(value);
	}
		
	public void setFfpBtnEnabled(boolean value) {
		_ffpBtn.setEnabled(value);
	}
	
	
	public JPanel getContentPane() {
		return _contentPane;
	}
	
	public JButton[][] getMazeGrids() {
		return _mazeGrids;
	}
	
	public void setMazeGrids(JButton[][] mazeGrids) {
		_mazeGrids = mazeGrids;
	}
	
	
	private void initContentPane(JPanel contentPane) {

		/*
		 * Add right panel: the reference map and two control buttons
		 * (load/clear).
		 */
		_mapPane = new JPanel(new FlowLayout());
		_mapPane.setPreferredSize(new Dimension(450, 650));
		JPanel map = new JPanel();
		map.setLayout(new GridLayout(Arena.MAP_WIDTH, Arena.MAP_LENGTH));
		map.setPreferredSize(new Dimension(450, 600));
		_mapGrids = new JButton[Arena.MAP_WIDTH][Arena.MAP_LENGTH];
		for (int x = 0; x < Arena.MAP_WIDTH; x++) {
			for (int y = 0; y < Arena.MAP_LENGTH; y++) {
				_mapGrids[x][y] = new JButton();
				
				if (RobotSystem.isRealRun()) {
					_mapGrids[x][y].setEnabled(false);
					_mapGrids[x][y].setBackground(Color.GRAY);
				} else {
					_mapGrids[x][y].setActionCommand("ToggleObstacleAt " + x + "," + y);
					_mapGrids[x][y].addActionListener(this);
					_mapGrids[x][y].setBorder(BorderFactory.createLineBorder(Color.GRAY));
					_mapGrids[x][y].setBackground(Color.GREEN);
					if ((x >= 0 & x <= 2 & y >= 12 & y <= 14) || (y >= 0 & y <= 2 & x >= 17 & x <= 19)) {
						_mapGrids[x][y].setEnabled(false);
						_mapGrids[x][y].setBackground(Color.ORANGE);
						if (x == 1 & y == 13) {
							_mapGrids[x][y].setText("G");
						} else if (x == 18 && y == 1) {
							_mapGrids[x][y].setText("S");
						}
					}
				}
				
				map.add(_mapGrids[x][y]);
			}
		}
		
		if (!RobotSystem.isRealRun()) {
			loadArenaFromDisk();
		}
		
		_mapPane.add(map);
		JButton loadMap = new JButton("Load");
		JButton clearMap = new JButton("Clear");
		
		if (RobotSystem.isRealRun()) {
			loadMap.setEnabled(false);
			clearMap.setEnabled(false);
		} else {
			loadMap.setActionCommand("LoadMap");
			loadMap.addActionListener(this);
			
			clearMap.setActionCommand("ClearMap");
			clearMap.addActionListener(this);
		}
		
		
		_mapPane.add(loadMap);
		_mapPane.add(clearMap);
		contentPane.add(_mapPane, BorderLayout.EAST);

		/*
		 * Add middle panel: the explore/fastest path control panel.
		 */

		// Add control switch (combo box).
		_ctrlPane = new JPanel(new BorderLayout());
		_ctrlPane.setBorder(new EmptyBorder(50, 20, 50, 20));
		String comboBoxItems[] = { EXPLORE_PANEL, FFP_PANEL };
		JComboBox cbCtrlSwitch = new JComboBox(comboBoxItems);
		cbCtrlSwitch.setFont(new Font("Tahoma", Font.BOLD, 16));
		cbCtrlSwitch.setEditable(false);
		cbCtrlSwitch.addActionListener(this);
		cbCtrlSwitch.setActionCommand("SwitchCtrl");
		_ctrlPane.add(cbCtrlSwitch, BorderLayout.NORTH);

		// Add control panel for exploring.
		JLabel[] exploreCtrlLabels = new JLabel[4];
		_exploreTextFields = new JTextField[4];
		
		_exploreBtn = new JButton("Explore");
		_stopExploreBtn = new JButton("Stop Exploration");
		_stopExploreBtn.setEnabled(false);
		
		if (RobotSystem.isRealRun()) {
			_exploreBtn.setEnabled(false);
		} else {
			_exploreBtn.setActionCommand("ExploreMaze");
			_exploreBtn.addActionListener(this);
			
			_stopExploreBtn.setActionCommand("stopExplore");
			_stopExploreBtn.addActionListener(this);
		}
		
		exploreCtrlLabels[0] = new JLabel("Robot initial position: ");
		exploreCtrlLabels[1] = new JLabel("Speed (steps/sec): ");
		exploreCtrlLabels[2] = new JLabel("Target coverage (%): ");
		exploreCtrlLabels[3] = new JLabel("Time limit (sec): ");
		for (int i = 0; i < 4; i++) {
			_exploreTextFields[i] = new JTextField(10);
			if (RobotSystem.isRealRun()) {
				_exploreTextFields[i].setEditable(false);
			}
		}
		
		JPanel exploreInputPane = new JPanel(new GridLayout(4, 2));
		
		exploreInputPane.add(exploreCtrlLabels[0]);
		exploreInputPane.add(_exploreTextFields[0]);
		exploreInputPane.add(exploreCtrlLabels[1]);
		exploreInputPane.add(_exploreTextFields[1]);
		exploreInputPane.add(exploreCtrlLabels[2]);
		exploreInputPane.add(_exploreTextFields[2]);
		exploreInputPane.add(exploreCtrlLabels[3]);
		exploreInputPane.add(_exploreTextFields[3]);
		
		if (!RobotSystem.isRealRun()) {
			_exploreTextFields[0].setEditable(false);
			exploreCtrlLabels[0].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_exploreTextFields[0].setText("2,2");
			_exploreTextFields[0].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_exploreTextFields[0].getDocument().addDocumentListener(new InitialPositionListener());
			_exploreTextFields[0].getDocument().putProperty("name", "Robot Initial Position");
			
			exploreCtrlLabels[1].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_exploreTextFields[1].setText("10");
			_exploreTextFields[1].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_exploreTextFields[1].getDocument().addDocumentListener(new InitialPositionListener());
			_exploreTextFields[1].getDocument().putProperty("name", "Robot Explore Speed");
			
			exploreCtrlLabels[2].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_exploreTextFields[2].setText("100");
			_exploreTextFields[2].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_exploreTextFields[2].getDocument().addDocumentListener(new InitialPositionListener());
			_exploreTextFields[2].getDocument().putProperty("name", "Target Coverage");
			
			exploreCtrlLabels[3].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_exploreTextFields[3].setText("360");
			_exploreTextFields[3].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_exploreTextFields[3].getDocument().addDocumentListener(new InitialPositionListener());
			_exploreTextFields[3].getDocument().putProperty("name", "Robot Explore Time Limit");
		}
		
		JPanel exploreBtnPane = new JPanel();
		exploreBtnPane.add(_exploreBtn);
		exploreBtnPane.add(_stopExploreBtn);
		
		JPanel exploreCtrlPane = new JPanel();
		exploreCtrlPane.add(exploreInputPane);
		exploreCtrlPane.add(exploreBtnPane);
		exploreCtrlPane.setBorder(new EmptyBorder(20, 20, 20, 20));

		// Add control panel for finding fastest path.
		JLabel[] ffpCtrlLabels = new JLabel[2];
		_ffpTextFields = new JTextField[2];
		_ffpBtn = new JButton("Navigate");
		
		if (RobotSystem.isRealRun()) {
			_ffpBtn.setEnabled(false);
		} else {
			_ffpBtn.setActionCommand("FindFastestPath");
			_ffpBtn.addActionListener(this);
			_ffpBtn.setEnabled(false);
		}
		
		ffpCtrlLabels[0] = new JLabel("Speed (steps/sec): ");
		ffpCtrlLabels[1] = new JLabel("Time limit (sec): ");
		for (int i = 0; i < 2; i++) {
			_ffpTextFields[i] = new JTextField(10);
			if (RobotSystem.isRealRun()) {
				_ffpTextFields[i].setEditable(false);
			}
		}

		JPanel ffpInputPane = new JPanel(new GridLayout(2, 2));
		ffpInputPane.add(ffpCtrlLabels[0]);
		ffpInputPane.add(_ffpTextFields[0]);
		ffpInputPane.add(ffpCtrlLabels[1]);
		ffpInputPane.add(_ffpTextFields[1]);
		
		if (!RobotSystem.isRealRun()) {
			ffpCtrlLabels[0].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_ffpTextFields[0].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_ffpTextFields[0].setEditable(false);
	
			_ffpTextFields[1].setText("120");
			ffpCtrlLabels[1].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_ffpTextFields[1].setFont(new Font("Tahoma", Font.PLAIN, 14));
			_ffpTextFields[1].getDocument().addDocumentListener(new InitialPositionListener());
			_ffpTextFields[1].getDocument().putProperty("name", "Robot FFP Time Limit");
	}

		JPanel ffpBtnPane = new JPanel();
		ffpBtnPane.add(_ffpBtn);

		JPanel ffpCtrlPane = new JPanel();
		ffpCtrlPane.add(ffpInputPane);
		ffpCtrlPane.add(ffpBtnPane);
		ffpCtrlPane.setBorder(new EmptyBorder(20, 20, 20, 20));

		// Add card panel to switch between explore and shortest path panels.
		JPanel cardPane = new JPanel(new CardLayout());
		cardPane.add(exploreCtrlPane, EXPLORE_PANEL);
		cardPane.add(ffpCtrlPane, FFP_PANEL);
		cardPane.setPreferredSize(new Dimension(280, 300));
		_ctrlPane.add(cardPane, BorderLayout.CENTER);

		// Add status panel.
		JPanel statusPane = new JPanel(new BorderLayout());
		JLabel statusLabel = new JLabel("Status Console:");
		statusPane.add(statusLabel, BorderLayout.NORTH);
		JPanel statusConsole = new JPanel(new GridLayout(3, 1));
		statusConsole.setBackground(Color.LIGHT_GRAY);
		statusConsole.setPreferredSize(new Dimension(280, 100));
		_status = new JLabel("waiting for commands...");
		_status.setHorizontalAlignment(JLabel.CENTER);
		_timer = new JLabel();
		_timer.setHorizontalAlignment(JLabel.CENTER);
		_coverageUpdate = new JLabel();
		_coverageUpdate.setHorizontalAlignment(JLabel.CENTER);
		statusConsole.add(_status);
		statusConsole.add(_coverageUpdate);
		statusConsole.add(_timer);
		statusPane.add(statusConsole, BorderLayout.CENTER);
		_ctrlPane.add(statusPane, BorderLayout.SOUTH);

		contentPane.add(_ctrlPane, BorderLayout.CENTER);

		/*
		 * Add left panel: the maze panel.
		 */
		_mazePane = new JPanel(new FlowLayout());
		_mazePane.setPreferredSize(new Dimension(450, 650));
		JPanel maze = new JPanel();
		maze.setLayout(new GridLayout(Arena.MAP_WIDTH, Arena.MAP_LENGTH));
		maze.setPreferredSize(new Dimension(450, 600));
		_mazeGrids = new JButton[Arena.MAP_WIDTH][Arena.MAP_LENGTH];
		for (int x = 0; x < Arena.MAP_WIDTH; x++) {
			for (int y = 0; y < Arena.MAP_LENGTH; y++) {
				_mazeGrids[x][y] = new JButton();
				_mazeGrids[x][y].setEnabled(false);
				_mazeGrids[x][y].setBorder(BorderFactory.createLineBorder(Color.GRAY));
				if (x == 10) {
					_mazeGrids[x][y]
							.setBorder(new CompoundBorder(BorderFactory.createMatteBorder(3, 0, 0, 0, Color.BLUE),
									BorderFactory.createMatteBorder(0, 1, 1, 1, Color.GRAY)));
				}
				_mazeGrids[x][y].setBackground(Color.BLACK);
				maze.add(_mazeGrids[x][y]);
				if ((x >= 0 & x <= 2 & y >= 12 & y <= 14) || (y >= 0 & y <= 2 & x >= 17 & x <= 19)) {
					_mazeGrids[x][y].setEnabled(false);
					_mazeGrids[x][y].setBackground(Color.ORANGE);
					if (x == 1 & y == 13) {
						_mazeGrids[x][y].setText("G");
					} else if (x == 18 && y == 1) {
						_mazeGrids[x][y].setText("S");
					}
				}
			}
		}
		_mazePane.add(maze);
		contentPane.add(_mazePane, BorderLayout.WEST);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		String cmd = e.getActionCommand();
		_controller = Controller.getInstance();
		if (cmd.matches("ToggleObstacleAt [0-9]+,[0-9]+")) {
			int index = cmd.indexOf(",");
			int x = Integer.parseInt(cmd.substring(17, index));
			int y = Integer.parseInt(cmd.substring(index + 1));
			_controller.toggleObstacle(_mapGrids, x, y);
			System.out.println("Toggle at "+x+", "+y);
		} else if (cmd.equals("SwitchCtrl")) {
			JComboBox cb = (JComboBox) e.getSource();
			JPanel cardPanel = (JPanel) _ctrlPane.getComponent(1);
			_controller.switchComboBox(cb, cardPanel);
		} else if (cmd.equals("LoadMap")) {
			_controller.loadMap(_mapGrids);
		} else if (cmd.equals("ClearMap")) {
			_controller.clearMap(_mapGrids);
		} else if (cmd.equals("ExploreMaze")) {
			_exploreBtn.setEnabled(false);
			_stopExploreBtn.setEnabled(true);
        	_controller.exploreMaze();
		} else if (cmd.equals("FindFastestPath")) {
			_ffpBtn.setEnabled(false);
			_controller.findFastestPath();
		} else if(cmd.equals("stopExplore")) {
			_stopExploreBtn.setEnabled(false);
			_controller.stopExploring();
			
		}
	}

	public void setStatus(String message) {
		_status.setText(message);
	}

	public void setTimer (int timeLeft) {
		_timer.setText("Time left (sec): " + timeLeft);
	}
	
	public String getTimerMessage () {
		return _timer.getText();
	}
	
	public void setTimerMessage (String message) {
		_timer.setText(message);
	}
	
	public void setCoverageUpdate (Float coverage) {
		_coverageUpdate.setText("Coverage (%): " + 	String.format("%.1f", coverage));
	}
	
	public void setCoverageUpdate (String message) {
		_coverageUpdate.setText(message);
	}
	
	/*
	 * Add a document listener class to dynamically show robot position.
	 */
	class InitialPositionListener implements DocumentListener {
		public void insertUpdate(DocumentEvent e) {
			update(e);
		}

		public void removeUpdate(DocumentEvent e) {
			update(e);
		}

		public void changedUpdate(DocumentEvent e) {

		}

		private void update(DocumentEvent e) {
			_controller = Controller.getInstance();
			Document doc = (Document) e.getDocument();
			String name = (String) doc.getProperty("name");
			if (name.equals("Robot Initial Position")) {
				try {
					String position = doc.getText(0, doc.getLength());
					if (position.matches("[0-9]+,[0-9]+")) {
						int index = position.indexOf(",");
						int x = Integer.parseInt(position.substring(0, index));
						int y = Integer.parseInt(position.substring(index + 1));
						_controller.resetRobotInMaze(_mazeGrids, x, y);
					} else {
						_controller.resetMaze(_mazeGrids);
						_status.setText("robot initial position not set");
					}
				} catch (BadLocationException ex) {
					ex.printStackTrace();
				}
			} else if (name.equals("Robot Explore Speed")) {
				try {
					String speed = doc.getText(0, doc.getLength());
					if (speed.matches("[0-9]+")) {
						_controller.setRobotSpeed(Integer.parseInt(speed));
						_ffpTextFields[0].setText(speed);
					} else {
						_status.setText("robot speed not set");
					}
				} catch (BadLocationException ex) {
					ex.printStackTrace();
				}
			} else if (name.equals("Target Coverage")) {
				try {
					String coverage = doc.getText(0, doc.getLength());
					if (coverage.matches("[0-9]+")) {
						_controller.setCoverage(Integer.parseInt(coverage));
						_coverageUpdate.setText("Coverage (%): 0");
					} else {
						_status.setText("target coverage not set");
					}
				} catch (BadLocationException ex) {
					ex.printStackTrace();
				}
			} else if (name.equals("Robot Explore Time Limit")) {
				try {
					String timeLimit = doc.getText(0, doc.getLength());
					if (timeLimit.matches("[0-9]+")) {
						_controller.setExploreTimeLimit(Integer.parseInt(timeLimit));
						_timer.setText("Time left (sec): " + timeLimit);
					} else {
						_status.setText("time limit for exploring not set");
					}
				} catch (BadLocationException ex) {
					ex.printStackTrace();
				}
			} else if (name.equals("Robot FFP Time Limit")) {
				try {
					String timeLimit = doc.getText(0, doc.getLength());
					if (timeLimit.matches("[0-9]+")) {
						_controller.setFFPTimeLimit(Integer.parseInt(timeLimit));
						_coverageUpdate.setText("");
						_timer.setText("Time left (sec): " + timeLimit);
					} else {
						_status.setText("time limit for fastest path not set");
					}
				} catch (BadLocationException ex) {
					ex.printStackTrace();
				}
			}
		}

	}
	//Arena.MAP_LENGTH * i gives me each row, j gives me position in each row
	//_mapGrids array will be exactly the same as the map in the UI
	private void loadArenaFromDisk() {
		FileReaderWriter fileReader;
		try {
			fileReader = new FileReaderWriter(FileSystems.getDefault().getPath(Controller.ARENA_DESCRIPTOR_PATH));
			String arenaDescriptor = fileReader.read();
			System.out.println(arenaDescriptor);
			if (!arenaDescriptor.equals("")) {
				for (int i = 0; i < Arena.MAP_WIDTH; i++) {
					for (int j = 0; j < Arena.MAP_LENGTH; j++) {
						if (arenaDescriptor.charAt(Arena.MAP_LENGTH * i + j) == '1') {
							_mapGrids[19 - i][j].setBackground(Color.RED);
						}
					}
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		Arena arena = Arena.getInstance();
		arena.setLayout(_mapGrids);
	}

	public void refreshExploreInput() {
		for (int i = 0; i < 4; i++) {
			_exploreTextFields[i].setText(_exploreTextFields[i].getText());
		}
	}
	
	public void refreshFfpInput() {
		for (int i = 0; i < 2; i++) {
			_ffpTextFields[i].setText(_ffpTextFields[i].getText());
		}

	}
	
	// Check if Explorer Input is valid
	public boolean isIntExploreInput() {

		String[] exploreInput = new String[4];
		for (int i = 0; i < 4; i++) {
			exploreInput[i] = _exploreTextFields[i].getText();
		}
		
		if (! exploreInput[0].matches("[0-9]+,[0-9]+")) {
			return false;
		} else {
			int posX, posY, index;
			index = exploreInput[0].indexOf(",");
			posX = Integer.parseInt(exploreInput[0].substring(0, index));
			posY = Integer.parseInt(exploreInput[0].substring(index+1));
			if (posX > Arena.MAP_LENGTH || posY > Arena.MAP_WIDTH) {
				return false;
			}
		}
		
		if (!exploreInput[1].matches("[0-9]+") || !exploreInput[2].matches("[0-9]+") ||
				!exploreInput[3].matches("[0-9]+")) {
			return false;
		}
		
		if (exploreInput[2].matches("[0-9]+")) {
			int coverage;
			coverage = Integer.parseInt(exploreInput[2]);
			if (coverage > 100) {
				return false;
			}
		}
		
		return true;
	}

	public boolean isIntFFPInput() {
		String[] ffpInput = new String[2];
		for (int i = 0; i < 2; i++) {
			ffpInput[i] = _ffpTextFields[i].getText();
		}
		
		if (! ffpInput[0].matches("[0-9]+") || ! ffpInput[1].matches("[0-9]+")) {
			return false;
		}
		
		return true;
	}
	
}
