package mdp14.mdp14app;

import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
//import android.support.v4.view.MotionEventCompat;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Toast;

import mdp14.mdp14app.model.Map;
import mdp14.mdp14app.model.Position;
import mdp14.mdp14app.model.Robot;
import mdp14.mdp14app.model.WayPoint;

import static android.view.MotionEvent.INVALID_POINTER_ID;

public class MapCanvas extends View implements View.OnTouchListener {
    final float scale = getResources().getDisplayMetrics().density;

    Paint separator = new Paint();      //Horizontal and vertical lines on Grid
    Paint startEndPoint = new Paint();  //Start and End points
    Paint exploredArea = new Paint();   //Explored area
    Paint robot = new Paint();          //Robot
    Paint robotEye = new Paint();       //Robot eye
    Paint obstacle = new Paint();       //Obstacles
    Paint waypoint = new Paint();       //Waypoint
    Paint unexploredArea = new Paint(); //Unexplored area

    float padding = 50;
    float paddingX = 50;
    float paddingY = 0;
    float w = this.getWidth()-2*50;
    float h = this.getHeight()-50;
    float cellWidth = w/15f;
    float cellHeight = h/20f;

    int lastX;
    int lastY;
    String toastText;
    Toast mCurrentToast = null;

    private GestureDetector  mDetector;
    private Activity mScaleDetector;

    public MapCanvas(Context context) {
        super(context);
        separator.setColor(Color.parseColor("#212121"));
        exploredArea.setColor(Color.parseColor("#FFEB3B"));
        obstacle.setColor(Color.BLACK);
        robot.setColor(Color.parseColor("#F03958")); //#fb3958 #A401FD
        robotEye.setColor((Color.parseColor("#E0E0E0")));
        unexploredArea.setColor(Color.parseColor("#458eff"));   //#0696D7
        waypoint.setColor(Color.parseColor("#00E40B"));
        startEndPoint.setColor(Color.parseColor("#FBC02D"));

    }

    public void setGesture(Context context){
        mDetector = new GestureDetector (context, new MyGestureListener());
    }

    //onDraw is ran when android render the view on the phone
    protected void onDraw(Canvas canvas) {
        //calculation for drawing the grids
        padding = 40;
        paddingX = padding;
        paddingY = 0;
        h = this.getHeight()-padding;
        w = this.getWidth()-2*padding;
        if(h/20f<w/15f){ //adjusting the map cell when in different oritentation
            w = h/20f*15f;
        }else{
            h= w/15f*20f;
        }

        paddingX = (this.getWidth()-w)/2f;
        cellWidth = w/15f;
        cellHeight = h/20f;


        //draw background (unexlpored)
        canvas.drawRect(paddingX,  paddingY, paddingX + w,  paddingY + h, unexploredArea);

        //draw explored & obstacle
        drawExploredTile(canvas);

        //draw waypoint
        drawWaypoint(canvas);

        //draw robot
        drawRobot(canvas);
    }


    private void drawWaypoint(Canvas canvas) {
        Position wp = WayPoint.getInstance().getPosition();
        if(wp!=null&&wp.getPosX()>=0&&wp.getPosX()<15&&wp.getPosY()<20&&wp.getPosY()>=0){
            float posX = (paddingX+wp.getPosX()*cellWidth);
            float posY = (paddingY+(19-wp.getPosY())*cellHeight);
             canvas.drawRect(posX, posY, posX+cellWidth, posY+cellHeight, waypoint);
        }
    }

    private void drawRobot(Canvas canvas) {
        Robot r = Robot.getInstance();
        if(r.getPosX()<1||r.getPosY()<1||r.getPosX()>13||r.getPosY()>18){
            return;
        }
        float cellWidth = w/15f;
        float cellHeight = h/20f;
        float xCenterPosition = r.getPosX()*cellWidth+ paddingX  +(cellWidth/2f);
        float yCenterPosition = (19f-r.getPosY())*cellWidth+ paddingY  +(cellHeight/2f);
        canvas.drawCircle(xCenterPosition, yCenterPosition,cellWidth*1.3f, robot);

        //draw the front of the robot
        float direction = r.getDirection();
        double radians = Math.toRadians(direction);
        float sensorCenterX = (float) (xCenterPosition+(cellWidth/1.5f*Math.sin(radians)));
        float sensorCenterY = (float) (yCenterPosition-(cellWidth/1.5f*Math.cos(radians)));
        canvas.drawCircle(sensorCenterX, sensorCenterY,cellWidth/3f, robotEye);
    }

    // draw tiles
    private void drawExploredTile(Canvas canvas) {
        //draw explored
        int[][]explored = Map.getInstance().getExploredTiles();
        int[][]obstacles = Map.getInstance().getObstacles();
        for(int x =0;x<15;x++){
            for(int y =0;y<20;y++){
              if( explored[y][x] == 1){
                  float posX = (paddingX+x*cellWidth);
                  float posY = (paddingY+(19-y)*cellHeight);
                  if(obstacles[y][x]==1){
                      //draw obstacles
                      canvas.drawRect(posX, posY, posX+cellWidth, posY+cellHeight, obstacle);
                  }else{
                      if((y==0&&x==0)||(y==0&&x==1)||(y==0&&x==2)||   (y==1&&x==0)||  (y==1&&x==1)||   (y==1&&x==2)|| (y==2&&x==0)||(y==2&&x==1)|| (y==2&&x==2)||
  (y==19&&x==14)||   (y==19&&x==13)||   (y==19&&x==12)||    (y==18&&x==14)||   (y==18&&x==13)||     (y==18&&x==12)|| (y==17&&x==14)||    (y==17&&x==13)||  (y==17&&x==12)){
                         //if it is start or end point show light gray
                          canvas.drawRect(posX, posY, posX+cellWidth, posY+cellHeight, startEndPoint);
                      }else{
                          //show white for explored area
                          canvas.drawRect(posX, posY, posX+cellWidth, posY+cellHeight, exploredArea);
                      }
                  }
              }
            }
        }
        //drawlines for grids, horizontal and vertical
        for(int i = 0;i<16;i++){
            canvas.drawLine((float)i*(w/15f)+ paddingX, paddingY, i*(w/15f)+ paddingX, paddingY +h, separator);
        }
        for(int i = 0;i<21;i++){
            canvas.drawLine(paddingX, i*(h/20f)+ paddingY, paddingX +w,i*(h/20f)+ paddingY, separator);
        }
    }


    @Override
    public boolean onTouch(View view, MotionEvent me) {
        //calculation to get the tapped position from view geometry location

        try {
            Thread.sleep(50);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        me.getSource();

        float X;
        float Y;

        float selectedX;
        float selectedY;
        float cellWidth = w/15f;
        float cellHeight = h/20f;

        int posX;
        int posY;

        MainActivity ma = (MainActivity) this.getContext();

        switch(me.getAction()) {
            case MotionEvent.ACTION_DOWN:
                X = me.getX();
                Y = me.getY();
                selectedX = X- paddingX;
                selectedY = Y- paddingY;
                posX = (int)(selectedX/cellWidth);
                posY = 19-(int)(selectedY/cellHeight);
                lastX = posX;
                lastY = posY;
                toastText = "tapped " + posX + ", " + posY;
                //showToast(toastText);

                break;

            case MotionEvent.ACTION_MOVE: {
                X = me.getX();
                Y = me.getY();
                selectedX = X- paddingX;
                selectedY = Y- paddingY;
                posX = (int)(selectedX/cellWidth);
                posY = 19-(int)(selectedY/cellHeight);
                lastX = posX;
                lastY = posY;
                toastText = "tapped " + posX + ", " + posY;

                break;
            }

            case MotionEvent.ACTION_POINTER_UP: {
                X = me.getX();
                Y = me.getY();
                selectedX = X- paddingX;
                selectedY = Y- paddingY;
                posX = (int)(selectedX/cellWidth);
                posY = 19-(int)(selectedY/cellHeight);
                lastX = posX;
                lastY = posY;
                toastText = "tapped " + posX + ", " + posY;
                //showToast(toastText);


                break;
            }
        }
        showToast(toastText);
        ma.onGridTapped( lastX,  lastY);


        /*switch(me.getAction()) {
            case MotionEvent.ACTION_DOWN:
                float inX = me.getX();
                float inY = me.getY();

                float selectedX = inX- paddingX;
                float selectedY = inY- paddingY;
                float cellWidth = w/15f;
                float cellHeight = h/20f;
                int posX = (int)(selectedX/cellWidth);
                int posY = 19-(int)(selectedY/cellHeight);
                String text = "tapped " + posX + ", " + posY;
                showToast(text);
                MainActivity ma = (MainActivity) this.getContext();
                ma.onGridTapped( posX,  posY);
                break;

            case MotionEvent.ACTION_MOVE: {
                float newinX = me.getX();
                float newinY = me.getY();
                //int newduration = Toast.LENGTH_SHORT;


                float newselectedX = newinX- paddingX;
                float newselectedY = newinY- paddingY;
                float newcellWidth = w/15f;
                float newcellHeight = h/20f;
                int newposX = (int)(newselectedX/newcellWidth);
                int newposY = 19-(int)(newselectedY/newcellHeight);
                String newtext = "tapped " + newposX + ", " + newposY;
                showToast(newtext);
                MainActivity newma = (MainActivity) this.getContext();
                newma.onGridTapped( newposX,  newposY);
                break;
            }

            case MotionEvent.ACTION_POINTER_UP: {
                float upX = me.getX();
                float upY = me.getY();
                float upselectedX = upX- paddingX;
                float upselectedY = upY- paddingY;
                float upcellWidth = w/15f;
                float upcellHeight = h/20f;
                int upposX = (int)(upselectedX/upcellWidth);
                int upposY = 19-(int)(upselectedY/upcellHeight);
                String uptext = "tapped " + upposX + ", " + upposY;
                showToast(uptext);
                MainActivity upma = (MainActivity) this.getContext();
                upma.onGridTapped( upposX,  upposY);
                break;
            }
        }
        */
        return true;
    }

    //Show toast

    public void showToast(String text) {


        if (mCurrentToast != null) {
            mCurrentToast.cancel();
        }

        Context context = getContext();
        mCurrentToast = Toast.makeText(context,text, Toast.LENGTH_SHORT);
        //mCurrentToast.setText(text);
        mCurrentToast.show();
    }

    /*
    @Override
    public boolean onTouch(View view, MotionEvent me) {
        //calculation to get the tapped position from view geometry location
        try {
            Thread.sleep(50);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        me.getSource();
        switch(me.getAction()) {
            case MotionEvent.ACTION_DOWN:
                float inX = me.getX();
                float inY = me.getY();
                Context context = getContext();
                int duration = Toast.LENGTH_SHORT;


                float selectedX = inX- paddingX;
                float selectedY = inY- paddingY;
                float cellWidth = w/15f;
                float cellHeight = h/20f;
                int posX = (int)(selectedX/cellWidth);
                int posY = 19-(int)(selectedY/cellHeight);
                Toast toast = Toast.makeText(context,"tapped: "+posX+","+posY, duration);
                toast.show();
                MainActivity ma = (MainActivity) this.getContext();
                ma.onGridTapped( posX,  posY);
                break;
        }
        return true;
    }
    */




    @Override
    public boolean onTouchEvent(MotionEvent event){
        this.mDetector.onTouchEvent(event);
        return super.onTouchEvent(event);
    }


    @Override
    public boolean dispatchTouchEvent(MotionEvent e)
    {
        super.dispatchTouchEvent(e);
        if(mDetector==null){
            return true;
        }
        return mDetector.onTouchEvent(e);
    }

    //listener for gesture
    class MyGestureListener extends GestureDetector.SimpleOnGestureListener {

        private static final int SWIPE_THRESHOLD = 100;
        private static final int SWIPE_VELOCITY_THRESHOLD = 100;
        @Override
        public boolean onDown(MotionEvent event) {
            return true;
        }

        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
            boolean result = false;
            try {
                float diffY = e2.getY() - e1.getY();
                float diffX = e2.getX() - e1.getX();
                if (Math.abs(diffX) > Math.abs(diffY)) {
                    if (Math.abs(diffX) > SWIPE_THRESHOLD && Math.abs(velocityX) > SWIPE_VELOCITY_THRESHOLD) {
                        if (diffX > 0) {
                            onSwipeRight();
                        } else {
                            onSwipeLeft();
                        }
                        result = true;
                    }
                }
                else if (Math.abs(diffY) > SWIPE_THRESHOLD && Math.abs(velocityY) > SWIPE_VELOCITY_THRESHOLD) {
                    if (diffY > 0) {
                        onSwipeBottom();
                    } else {
                        onSwipeTop();
                    }
                    result = true;
                }
            } catch (Exception exception) {
                exception.printStackTrace();
            }
            return result;
        }
    }



    public void onSwipeRight() {
        MainActivity ma = (MainActivity) this.getContext();
        ma.onSwipeRight();
    }

    public void onSwipeLeft() {
        MainActivity ma = (MainActivity) this.getContext();
        ma.onSwipeLeft();
    }

    public void onSwipeTop() {
        MainActivity ma = (MainActivity) this.getContext();
        ma.onSwipeTop();
    }

    public void onSwipeBottom() {
        MainActivity ma = (MainActivity) this.getContext();
        ma.onSwipeBottom();
    }
}
