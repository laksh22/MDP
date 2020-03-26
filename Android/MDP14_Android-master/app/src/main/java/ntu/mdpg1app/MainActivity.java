package ntu.mdpg1app;

import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridLayout;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;

import ntu.mdpg1app.bluetooth.BluetoothChatFragment;
import ntu.mdpg1app.model.HexBin;
import ntu.mdpg1app.model.IDblock;
import ntu.mdpg1app.model.Map;
import ntu.mdpg1app.model.Position;
import ntu.mdpg1app.model.Robot;
import ntu.mdpg1app.model.WayPoint;

public class MainActivity extends AppCompatActivity {
    GridLayout base_layout;
    BluetoothChatFragment fragment;
    MenuItem menu_set_robot_position;
    MenuItem menu_set_waypoint;
    MenuItem menu_remove_waypoint;

    MenuItem menu_update_map;
    MenuItem menu_auto_update_map;
    MenuItem menu_set_config1;
    MenuItem menu_set_config2;
    MenuItem menu_enable_swipe_input;
    MenuItem menu_show_bluetooth_chat;
    MenuItem menu_show_data_strings;


    TextView tv_status;
    Button btn_forward;
    Button btn_left;
    Button btn_right;
    Button btn_terminate;
    Button btn_explr;
    Button btn_fastest;
    Button btn_config1;
    Button btn_config2;

    String status = "Idle";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //initialize all the buttons
        tv_status = (TextView) findViewById(R.id.tv_status);
        btn_forward = (Button) findViewById(R.id.btn_forward);
        btn_left= (Button) findViewById(R.id.btn_left);
        btn_right= (Button) findViewById(R.id.btn_right);
        btn_terminate= (Button) findViewById(R.id.btn_terminate);
        btn_explr= (Button) findViewById(R.id.btn_explr);
        btn_fastest= (Button) findViewById(R.id.btn_fastest);
        btn_config1= (Button) findViewById(R.id.btn_config1);
        btn_config2= (Button) findViewById(R.id.btn_config2);
        base_layout = (GridLayout) findViewById(R.id.base_layout);

        //initialize the bluetooth service component
        if (savedInstanceState == null) {
            FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
            fragment = new BluetoothChatFragment();
            transaction.replace(R.id.sample_content_fragment, fragment);
            transaction.commit();
        }

        //initialize default status "idle"
        updateStatus(status);

        //initialize listener for all the buttons
        setBtnListener();

        //initialize the grid
        loadGrid();
    }

    //method to update the label textview
    public void updateStatus(String status){
        this.status = status;
        tv_status.setText(status);
    }

    //method to set all the event for buttons
    private void setBtnListener(){
        btn_forward.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                //check if robot is out of bounds
                if(!Robot.getInstance().isOutOfBounds()) {
                    Robot.getInstance().moveForward(10);
                    outgoingMessage("W", 1);
                    loadGrid();
                }
            }
        });
        btn_left.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Robot.getInstance().rotateLeft();
                outgoingMessage("A", 1);
                loadGrid();
            }
        });
        btn_right.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Robot.getInstance().rotateRight();
                outgoingMessage("D", 1);
                loadGrid();
            }
        });
        btn_terminate.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                outgoingMessage("X", 0);
                updateStatus(STATUS_TERMINATE_DESC);
            }
        });
        btn_explr.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                outgoingMessage("BEGINEX", 0);
                updateStatus(STATUS_EX_DESC);
            }
        });
        btn_fastest.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if (WayPoint.wp.getPosition() == null) {
                    status = "Setting WayPoint";
                    updateStatus(status);

                    menu_set_robot_position.setChecked(false);
                    menu_set_waypoint.setChecked(true);
                    Toast toast=Toast.makeText(getApplicationContext(),"Tap the Grid to set WayPoint",Toast.LENGTH_LONG);
                    toast.show();

                }else{
                    outgoingMessage("BEGINFP", 0);
                    updateStatus(STATUS_FP_DESC);
                }


            }
        });
        btn_config1.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                SharedPreferences prefs = getSharedPreferences(String.valueOf(R.string.app_name), MODE_PRIVATE);
                String retrievedText = prefs.getString("string1", null);
                if (retrievedText != null) {
                    outgoingMessage(retrievedText);
                }
            }
        });
        btn_config2.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                SharedPreferences prefs = getSharedPreferences(String.valueOf(R.string.app_name), MODE_PRIVATE);
                String retrievedText = prefs.getString("string2", null);
                if (retrievedText != null) {
                    outgoingMessage(retrievedText);
                }
            }
        });
    }

    //initalize all the menu item button
    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        super.onPrepareOptionsMenu(menu);
        menu_set_robot_position = menu.findItem(R.id.action_set_robot_position);
        menu_set_waypoint = menu.findItem(R.id.action_set_waypoint);
        menu_remove_waypoint = menu.findItem(R.id.action_remove_waypoint);
        menu_update_map = menu.findItem(R.id.action_update_map);
        menu_auto_update_map = menu.findItem(R.id.action_auto_update_map);
        menu_set_config1 = menu.findItem(R.id.action_set_config_string1);
        menu_set_config2 = menu.findItem(R.id.action_set_config_string2);
        menu_show_data_strings = menu.findItem(R.id.action_view_data_strings);
        menu_enable_swipe_input = menu.findItem(R.id.action_enable_swipe_input);
        menu_show_bluetooth_chat = menu.findItem(R.id.action_show_bluetooth_chat);
        return true;
    }

    //load/reload the Grid View into the page
    private void loadGrid(){
        MapCanvas mCustomDrawableView = new MapCanvas(this);
        //set touch event and swipe event
        if(menu_enable_swipe_input!=null&&menu_enable_swipe_input.isChecked()){
            mCustomDrawableView.setGesture(this);
        }else{
            mCustomDrawableView.setOnTouchListener(mCustomDrawableView);
        }
        base_layout.removeAllViews();
        base_layout.addView(mCustomDrawableView);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    //this is the event when menu item is clicked
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        //noinspection SimplifiableIfStatement

        if (id == R.id.action_set_robot_position) {
            boolean checked = item.isChecked();
            clearAllEditableCheckbox();
            item.setChecked(!checked);
            return true;
        }
        if (id == R.id.action_set_waypoint) {
            boolean checked = item.isChecked();
            clearAllEditableCheckbox();
            item.setChecked(!checked);
            return true;
        }
        if (id == R.id.action_remove_waypoint) {
            removeWaypoint();
            return true;
        }
        if (id == R.id.action_update_map) {
            loadGrid();
            return true;
        }
        if (id == R.id.action_auto_update_map) {
            boolean checked = item.isChecked();
            item.setChecked(!checked);
            if(item.isChecked()){
                loadGrid();
            }
            return true;
        }
        if (id == R.id.action_set_config_string1) {
            setConfiguredString(1);
            return true;
        }
        if (id == R.id.action_set_config_string2) {
            setConfiguredString(2);
            return true;
        }

        if (id == R.id.action_view_data_strings) {
            displayDataStrings();
            return true;
        }

        if (id == R.id.action_enable_swipe_input) {
            boolean checked = item.isChecked();
            clearAllEditableCheckbox();
            item.setChecked(!checked);
            loadGrid();
            return true;
        }

        if (id == R.id.action_show_bluetooth_chat) {
            boolean checked = item.isChecked();
            item.setChecked(!checked);
            if(fragment!=null){
                fragment.showChat(!checked);
            }
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    //this method opens up a dialog to save input string on the phone
    //index is the key of the string.
    //eg string 1 is stored in 'string1', string 2 in 'string2'
    private void setConfiguredString(final int index){
        final EditText txtField = new EditText(this);
        SharedPreferences prefs = getSharedPreferences(String.valueOf(R.string.app_name), MODE_PRIVATE);
        String retrievedText = prefs.getString("string"+index, null);
        if (retrievedText != null) {
            txtField.setText(retrievedText);
        }

        new AlertDialog.Builder(this)
                .setTitle("Configure String "+index)
                .setView(txtField)
                .setPositiveButton("Save", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        String input = txtField.getText().toString();
                        SharedPreferences.Editor editor = getSharedPreferences(String.valueOf(R.string.app_name), MODE_PRIVATE).edit();
                        editor.putString("string"+index, input);
                        editor.apply();
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                    }
                })
                .show();
    }

    private void displayDataStrings(){

        // create an alert builder
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("MDF + Image String");

        // set the custom layout
        final View customLayout = getLayoutInflater().inflate(R.layout.data_strings, null);
        builder.setView(customLayout);

        // add a button
        builder.setPositiveButton("CLOSE", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                // send data from the AlertDialog to the Activity
                dialog.dismiss();
            }
        });

        // set the custom dialog components - text, image and button
        EditText text_mdf1 = (EditText) customLayout.findViewById(R.id.textbox_mdf1);
        String explored = HexBin.binToHex(Map.getInstance().getBinaryExplored());
        text_mdf1.setText(explored);

        EditText text_mdf2 = (EditText) customLayout.findViewById(R.id.textbox_mdf2);
        String obstacles = HexBin.binToHex(Map.getInstance().getBinaryExploredObstacle());
        //obstacles = obstacles + ";" + Map.getInstance().getObstacleStringFromAlgo();
        text_mdf2.setText(obstacles);

        EditText text_imgreg = (EditText) customLayout.findViewById(R.id.textbox_imgreg);
        String imgreg = "{";
        ArrayList<IDblock> numberedBlocks = Map.getInstance().getNumberedBlocks();
        for(IDblock blk : numberedBlocks){
            imgreg += String.format("(%s, %d, %d)", blk.getID(), blk.getPosition().getPosX(), blk.getPosition().getPosY());
            imgreg += ", ";
        }
        if(imgreg.length()>2)imgreg = imgreg.substring(0, imgreg.length()-2);
        imgreg += "}";
        text_imgreg.setText(imgreg);

        // create and show the alert dialog
        AlertDialog dialog = builder.create();
        dialog.show();
    }


    //this is to uncheck all the checkable menuitem
    private void clearAllEditableCheckbox(){
        menu_set_robot_position.setChecked(false);
        menu_set_waypoint.setChecked(false);
        menu_enable_swipe_input.setChecked(false);
    }

    public static final String STATUS_EX_DESC = "Moving (Exploring)";
    public static final String STATUS_FP_DESC = "Moving (Fastest Path)";
    public static final String STATUS_TERMINATE_DESC = "Terminated";

    //method is ran when new message comes in
    public void incomingMessage(String readMsg) {
        //update map

        final Robot r = Robot.getInstance();

        if(readMsg.length()>0){
            menu_show_bluetooth_chat.setChecked(true);
            fragment.showChat(true);
            String message[];
            // - delimiter for imgReg, : delimiter for everythig else
            if(readMsg.contains(":")) {
                message = readMsg.split(":");
            }else{
                message = readMsg.split("-");
            }

            if (message[0].equals("GRID")) { //receive mapDescriptor from Algo
                Map.getInstance().setMapJson(message[1]);
                if (menu_auto_update_map != null && menu_auto_update_map.isChecked()) {
                    loadGrid();
                }
            }

            else if (message[0].equals("DATA")) { //receive full data string (P1, P2, robot pos) from Algo
                String data[] = message[1].split(",");

                Map.getInstance().setMap(data[0], "", data[1]);

                r.setPosX(Float.parseFloat(data[2]));
                r.setPosY(Float.parseFloat(data[3]));
                r.setDirection(data[4]);

                if (menu_auto_update_map != null && menu_auto_update_map.isChecked()) {
                    loadGrid();
                }
            }

            else if (message[0].equals("BLOCK")) { //receive numbered block
                String data[] = message[1].split(","); //x, y, id
                    IDblock input = new IDblock(data[2], Integer.parseInt(data[0]), Integer.parseInt(data[1]));
                    Map.getInstance().addNumberedBlocks(input);
                    if (menu_auto_update_map != null && menu_auto_update_map.isChecked()) {
                        loadGrid();
                    }
                }


            else if (message[0].equals("ROBOTPOSITION")) { //receive robot position
                String posAndDirect[] = message[1].split(",");
                r.setPosX(Float.parseFloat(posAndDirect[0]));
                r.setPosY(Float.parseFloat(posAndDirect[1]));
                r.setDirection(posAndDirect[2]);

                if (menu_auto_update_map != null && menu_auto_update_map.isChecked()) {
                    loadGrid();
                }
            }
            else if(message[0].equals("S")){
                if (message[1].equals("F")) {
                    updateStatus("Moving Forward");
                }
                if (message[1].equals("TR")) {
                    updateStatus("Turning Right");
                }
                if (message[1].equals("TL")) {
                    updateStatus("Turning Left");
                }
                if (message[1].equals("FP")) {
                    updateStatus("Fastest Path");
                }
                if (message[1].equals("EX")) {
                    updateStatus("Exploration");
                }
                if (message[1].equals("DONE")) {
                    updateStatus("Done!");
                }
            }
            else if(message[0].trim().equals("Y")){ //harmonize with algo
                updateStatus("Moving");
            }
            else if(message[0].trim().equals("F")){ //harmonize with algo
                updateStatus("Done!");
            }
            else {
                updateStatus("Invalid Message");
                }
            }
    }


    //method to send out message to rpi thru bluetooth -kept for backward compatibility
    public boolean outgoingMessage(String sendMsg) {
        return fragment.sendMsg("@t" + sendMsg + "!");
    }
    public boolean outgoingMessage(String sendMsg, int destination) {
        //add delimiters
        // 0=algo/tcp, 1=ardu/serial
        if(destination == 0){
            sendMsg = "@t" + sendMsg + "!";
        }else if(destination == 1){
            sendMsg = "@s" + sendMsg + "|!";
        }
        return fragment.sendMsg(sendMsg);
    }
    //on the coordinate tapped
    //set waypoint, if menuitem is checked
    //set robot, if menuitem is checked
    public void onGridTapped(int posX, int posY) {
        if(menu_set_robot_position.isChecked()){
            Robot r = Robot.getInstance();
            r.setPosX(posX);
            r.setPosY(posY);
            r.setDirection("NORTH");
            outgoingMessage("START:"+(int)posX+","+(int)posY, 0);

            //Make a prompt here to confirm
            menu_set_robot_position.setChecked(false);
        }
        if(menu_set_waypoint.isChecked()){
            Position p = new Position(posX,posY);
            WayPoint.getInstance().setPosition(p);
            outgoingMessage("WP:"+(int)posX+","+(int)posY, 0);
            //Make a prompt here to confirm
            menu_set_waypoint.setChecked(false);
        }
        loadGrid();
    }

    //clear waypoint
    private void removeWaypoint(){
        WayPoint.getInstance().setPosition(null);
        loadGrid();
    }

    //swipe gesture input
    public void onSwipeTop() {

        //check if robot is out of bounds
        if(!Robot.getInstance().rotateToNorth()){
            if(!Robot.getInstance().isOutOfBounds()) {
                outgoingMessage("W", 1);
                //getack
                Robot.getInstance().moveForward(10);
            }
        }
        else{
            Integer count = Robot.getInstance().getCount();
            sendMsgOnSwipe(count);
            //getack
            Robot.getInstance().rotateRobotToNorth();
        }
        loadGrid();
    }

    public void onSwipeLeft() {
        if(!Robot.getInstance().rotateToWest()){
            if(!Robot.getInstance().isOutOfBounds()) {
                outgoingMessage("W", 1);
                //getack
                Robot.getInstance().moveForward(10);
            }
        }
        else{
            Integer count = Robot.getInstance().getCount();
            sendMsgOnSwipe(count);
            //getack
            Robot.getInstance().rotateRobotToWest();
        }

        loadGrid();
    }

    public void onSwipeRight() {
        if(!Robot.getInstance().rotateToEast()){
            if(!Robot.getInstance().isOutOfBounds()) {
                outgoingMessage("W", 1);
                //getack
                Robot.getInstance().moveForward(10);
            }
        }
        else{
            Integer count = Robot.getInstance().getCount();
            sendMsgOnSwipe(count);
            //getack
            Robot.getInstance().rotateRobotToEast();
        }
        loadGrid();
    }

    public void onSwipeBottom() {
        if(!Robot.getInstance().rotateToSouth()){
            if(!Robot.getInstance().isOutOfBounds()) {

                outgoingMessage("W", 1);
                //getack
                Robot.getInstance().moveForward(10);
            }
        }
        else{
            Integer count = Robot.getInstance().getCount();
            sendMsgOnSwipe(count);
            //getack
            Robot.getInstance().rotateRobotToSouth();
        }
        loadGrid();
    }

    public void sendMsgOnSwipe(Integer count){

        if(count==1){
            outgoingMessage("D", 1);
        }
        if(count==2){
            outgoingMessage("D", 1);
            outgoingMessage("D", 1);
        }
        if(count==-1){
            outgoingMessage("A", 1);
        }
    }

    @Override
    protected void onStop() {
        // call the superclass method first
        super.onStop();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

}

