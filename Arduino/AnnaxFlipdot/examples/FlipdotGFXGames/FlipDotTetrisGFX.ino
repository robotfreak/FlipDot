/*********************************************************************
  This sketch uses example for Adafruit Monochrome OLEDs based on SSD1306 drivers
  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98
  This example is for a 128x32 size display using I2C to communicate
  3 pins are required to interface (2 I2C and one reset)

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above, and the splash screen must be included in any redistribution

  Adopted for Annax Flipdot by robotfreak
*********************************************************************/

//int speakerPin = 8;

/* ========================== Tetris Game ========================= */
long delays = 0;
short delay_ = 500;
long bdelay = 0;
short buttondelay = 150;
short btdowndelay = 30;
short btsidedelay = 80;
unsigned char blocktype;
unsigned char offset;
unsigned char blockrotation;

#define rows 20
#define cols 10

boolean block[cols][rows+2]; //2 extra for rotation
boolean pile[cols][rows];
boolean disp[cols][rows];

boolean gameoverFlag = false;

unsigned long startTime;
unsigned long elapsedTime;
int cnt = 0;

void tetrisSetup() {

  Serial.println("Tetris");
  randomSeed(analogRead(0));

  // Display Arduino Pong splashscreen
  flipdot.setTextSize(1);
  flipdot.setTextColor(YELLOW);
  flipdot.setCursor(flipdot.width() / 2 - 20, 0);
  flipdot.println("Arduino");
  flipdot.setCursor(flipdot.width() / 2 - 20 / 2, 8);
  flipdot.println("Tetris");
  //flipdot.display();
  flipdot.update();
  delay(2000);

  newBlock();
  flipdot.update();

}

void tetrisLoop() {

  if (delays < millis())
  {
    delays = millis() + delay_;
    movedown();
  }

  //buttun actions
  int button = readBut();

  if (button == 1) //up=rotate
    rotate();
  if (button == 2) //right=moveright
    moveright();
  if (button == 3) //left=moveleft
    moveleft();
  if (button == 4) //down=movedown
    movedown();
}

//**********************************************************************************************************************************************************  
boolean moveleft()
{  
  
  if (space_left())
  {
    int i;
    int j;
    for (i=0;i<cols-1;i++)
    {
      for (j=0;j<rows;j++)      
      {
        block[i][j]=block[i+1][j];
      }
    }
    
    for (j=0;j<rows;j++)      
    {
      block[cols-1][j]=0;
    }    

    updateMatrix();
    return 1;
  }

  return 0;
}



//**********************************************************************************************************************************************************  
boolean moveright()
{
  
  if (space_right())
  {
    int i;
    int j;
    for (i=cols-1;i>0;i--)
    {
      for (j=0;j<rows;j++)      
      {
        block[i][j]=block[i-1][j];
      }
    }

    for (j=0;j<rows;j++)      
    {
      block[0][j]=0;
    }    
    
    updateMatrix();
   return 1;   
  
  }
  return 0;
}



//**********************************************************************************************************************************************************  
int readBut()
{
  if (bdelay > millis())
  {
    return 0;
  }
  if ((digitalRead(buttonLeft) == LOW))
  {
    //left
    bdelay = millis() + btsidedelay;    
    return 3;
  }
  
  if ((digitalRead(buttonDown) == LOW))
  {
    //down
    bdelay = millis() + btdowndelay;    
    return 4;
  }    
  if ((digitalRead(buttonRight) == LOW))
  {
    //right
    bdelay = millis() + btsidedelay;
    return 2;
  }  
  if ((digitalRead(buttonRotate) == LOW))
  {
    //rotate
    bdelay = millis() + buttondelay;
    return 1;
  }  
  
  return 0;
}



//**********************************************************************************************************************************************************  
void updateMatrix()
{
  int i;
  int j;  
  for (i=0;i<cols;i++)
  {
    for (j=0;j<rows;j++)
    {
      disp[i][j] = block[i][j] | pile[i][j];
      if (disp[i][j])
        flipdot.drawPixel(j,i,YELLOW);
      else
        flipdot.drawPixel(j,i,BLACK);
    }
  }
  flipdot.update();
}



//**********************************************************************************************************************************************************  
void rotate()
{
  
  //skip for square block(3)
  if (blocktype == 3) return;
  
  int xi;
  int yi;
  int i;
  int j;
  //detect left
  for (i=cols-1;i>=0;i--)
  {
    for (j=0;j<rows;j++)
    {
      if (block[i][j])
      {
        xi = i;
      }
    }
  }
  
  //detect up
  for (i=rows-1;i>=0;i--)
  {
    for (j=0;j<cols;j++)
    {
      if (block[j][i])
      {
        yi = i;
      }
    }
  }  
    
  if (blocktype == 0)
  {
    if (blockrotation == 0) 
    {
      
      
      if (!space_left())
      {
        if (space_right3())
        {
          if (!moveright())
            return;
          xi++;
        }
        else return;
      }     
      else if (!space_right())
      {
        if (space_left3())
        {
          if (!moveleft())
            return;
          if (!moveleft())
            return;          
          xi--;
          xi--;        
        }
        else
          return;
      }
      else if (!space_right2())
      {
        if (space_left2())
        {
          if (!moveleft())
            return;          
          xi--;      
        }
        else
          return;
      }   
   
      
      block[xi][yi]=0;
      block[xi][yi+2]=0;
      block[xi][yi+3]=0;      
      
      block[xi-1][yi+1]=1;
      block[xi+1][yi+1]=1;
      block[xi+2][yi+1]=1;      

      blockrotation = 1;
    }
    else
    {
      block[xi][yi]=0;
      block[xi+2][yi]=0;
      block[xi+3][yi]=0;
      
      block[xi+1][yi-1]=1;
      block[xi+1][yi+1]=1;
      block[xi+1][yi+2]=1;

      blockrotation = 0;
    }    
  }
  
  //offset to mid
  xi ++;  
  yi ++;  
  
  if (blocktype == 1)
  {
    if (blockrotation == 0)
    {
      block[xi-1][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;

      block[xi][yi-1] = 1;
      block[xi+1][yi-1] = 1;
      block[xi][yi+1] = 1;      
      
      blockrotation = 1;
    }
    else if (blockrotation == 1)
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }        
      xi--;
      
      block[xi][yi-1] = 0;
      block[xi+1][yi-1] = 0;
      block[xi][yi+1] = 0;      
      
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      block[xi+1][yi+1] = 1;      
      
      blockrotation = 2;      
    }
    else if (blockrotation == 2)
    {
      yi --;
      
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;
      block[xi+1][yi+1] = 0;      
      
      block[xi][yi-1] = 1;
      block[xi][yi+1] = 1;
      block[xi-1][yi+1] = 1;      
      
      blockrotation = 3;            
    }
    else
    {
      if (!space_right())
      {
        if (!moveleft())
          return;
        xi--;
      }
      block[xi][yi-1] = 0;
      block[xi][yi+1] = 0;
      block[xi-1][yi+1] = 0;        

      block[xi-1][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      
      blockrotation = 0;          
    }  
  }



  if (blocktype == 2)
  {
    if (blockrotation == 0)
    {
      block[xi+1][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;

      block[xi][yi-1] = 1;
      block[xi+1][yi+1] = 1;
      block[xi][yi+1] = 1;      
      
      blockrotation = 1;
    }
    else if (blockrotation == 1)
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }              
      xi--;
      
      block[xi][yi-1] = 0;
      block[xi+1][yi+1] = 0;
      block[xi][yi+1] = 0;      
      
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      block[xi-1][yi+1] = 1;      
      
      blockrotation = 2;      
    }
    else if (blockrotation == 2)
    {
      yi --;
      
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;
      block[xi-1][yi+1] = 0;      
      
      block[xi][yi-1] = 1;
      block[xi][yi+1] = 1;
      block[xi-1][yi-1] = 1;      
      
      blockrotation = 3;            
    }
    else
    {
      if (!space_right())
      {
        if (!moveleft())
          return;
        xi--;
      }      
      block[xi][yi-1] = 0;
      block[xi][yi+1] = 0;
      block[xi-1][yi-1] = 0;        

      block[xi+1][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      
      blockrotation = 0;          
    }  
  }
  
  if (blocktype == 4)
  {
    if (blockrotation == 0)
    {
      block[xi+1][yi-1] = 0;
      block[xi-1][yi] = 0;

      block[xi+1][yi] = 1;
      block[xi+1][yi+1] = 1;      
      
      blockrotation = 1;
    }
    else
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }              
      xi--;
      
      block[xi+1][yi] = 0;
      block[xi+1][yi+1] = 0;      
      
      block[xi-1][yi] = 1;
      block[xi+1][yi-1] = 1;
      
      blockrotation = 0;          
    }  
  }  


  if (blocktype == 5)
  {
    if (blockrotation == 0)
    {
      block[xi][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;

      block[xi][yi-1] = 1;
      block[xi+1][yi] = 1;
      block[xi][yi+1] = 1;      
      
      blockrotation = 1;
    }
    else if (blockrotation == 1)
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }              
      xi--;
      
      block[xi][yi-1] = 0;
      block[xi+1][yi] = 0;
      block[xi][yi+1] = 0;
      
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      block[xi][yi+1] = 1;
      
      blockrotation = 2;      
    }
    else if (blockrotation == 2)
    {
      yi --;
      
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;
      block[xi][yi+1] = 0;     
      
      block[xi][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi][yi+1] = 1;      
      
      blockrotation = 3;            
    }
    else
    {
      if (!space_right())
      {
        if (!moveleft())
          return;
        xi--;
      }      
      block[xi][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi][yi+1] = 0;      
      
      block[xi][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      
      blockrotation = 0;          
    }  
  }
  
  if (blocktype == 6)
  {
    if (blockrotation == 0)
    {
      block[xi-1][yi-1] = 0;
      block[xi][yi-1] = 0;

      block[xi+1][yi-1] = 1;
      block[xi][yi+1] = 1;      
      
      blockrotation = 1;
    }
    else
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }              
      xi--;
      
      block[xi+1][yi-1] = 0;
      block[xi][yi+1] = 0;      
      
      block[xi-1][yi-1] = 1;
      block[xi][yi-1] = 1;
      
      blockrotation = 0;          
    }  
  }  

  //if rotating made block and pile overlap, push rows up
  while (!check_overlap())
  {
    for (i=0;i<rows+2;i++)
    {
      for (j=0;j<cols;j++)
      {
         block[j][i] = block[j][i+1];
      }
    }
    delays = millis() + delay_;
  }
  
  
  updateMatrix();    
}



//**********************************************************************************************************************************************************  
void movedown()
{ 
  if (space_below())
  {
    //move down
    int i;
    for (i=rows-1;i>=0;i--)
    {
      int j;
      for (j=0;j<cols;j++)
      {
        block[j][i] = block[j][i-1];
      }
    }
    for (i=0;i<cols-1;i++)
    {
      block[i][0] = 0;
    }
  }
  else
  {
    //merge and new block
    int i;
    int j;    
    for (i=0;i<cols;i++)
    {
     for(j=0;j<rows;j++)
     {
       if (block[i][j])
       {
         pile[i][j]=1;
         block[i][j]=0;
       }
     }
    }
    newBlock();   
  }
  updateMatrix();  
}



//**********************************************************************************************************************************************************  
boolean check_overlap()
{
  int i;
  int j;  
  for (i=0;i<rows;i++)
  {
    for (j=0;j<cols-1;j++)
    {
       if (block[j][i])
       {
         if (pile[j][i])
           return false;
       }        
    }
  }
  for (i=rows;i<rows+2;i++)
  {
    for (j=0;j<cols-1;j++)
    {
       if (block[j][i])
       {
         return false;
       }        
    }
  }  
  return true;
}



//**********************************************************************************************************************************************************  
void check_gameover()
{
  int i;
  int j;
  int cnt=0;;
  
  for(i=rows-1;i>=0;i--)
  {
    cnt=0;
    for (j=0;j<cols;j++)
    {
      if (pile[j][i])
      {
        cnt ++;
      }
    }    
    if (cnt == cols)
    {
      for (j=0;j<cols;j++)
      {
        pile[j][i]=0;
      }        
      updateMatrix();
      delay(50);
      
      int k;
      for(k=i;k>0;k--)
      {
        for (j=0;j<cols;j++)
        {
          pile[j][k] = pile[j][k-1];
        }                
      }
      for (j=0;j<cols;j++)
      {
        pile[j][0] = 0;
      }        
      updateMatrix();      
      delay(50);      
      i++;     
      
      
    
    }
  }  
  
  
  for(i=0;i<cols;i++)
  {
    if (pile[i][0])
      gameover();
  }
  return;
}



//**********************************************************************************************************************************************************  
void gameover()
{
  int i;
  int j;
  

  gameoverFlag = true;
  startTime = millis();       
       
  delay(300);       
            
  while(true)      //To re-play if any buttons depressed again
  {      
    int button = readBut();
    
    if ((button < 5) && (button > 0))
    {
      gameoverFlag = false;    
    
      for(i=rows-1;i>=0;i--)
      {
        for (j=0;j<cols;j++)
        {
          pile[j][i]=0;
        }             
      }
    
      break;
    }  
  }  
}



//**********************************************************************************************************************************************************  
void newBlock()
{
  check_gameover();
  
  
  blocktype = random(7);
  offset = cols/2; //random(cols-4);
  
  if (blocktype == 0)
  // 0
  // 0
  // 0
  // 0
  {
    block[offset][0]=1;
    block[offset][1]=1;
    block[offset][2]=1;
    block[offset][3]=1;      
  }

  if (blocktype == 1)
  // 0
  // 0 0 0
  {
    block[offset][0]=1;
    block[offset][1]=1;
    block[offset+1][1]=1;
    block[offset+2][1]=1;        
  }
  
  if (blocktype == 2)
  //     0
  // 0 0 0
  {
    block[offset+2][0]=1;
    block[offset][1]=1;
    block[offset+1][1]=1;
    block[offset+2][1]=1;         
  }

  if (blocktype == 3)
  // 0 0
  // 0 0
  {
    block[offset][0]=1;
    block[offset][1]=1;
    block[offset+1][0]=1;
    block[offset+1][1]=1;          
  }    

  if (blocktype == 4)
  //   0 0
  // 0 0
  {
    block[offset+1][0]=1;
    block[offset+2][0]=1;
    block[offset][1]=1;
    block[offset+1][1]=1;         
  }    
  
  if (blocktype == 5)
  //   0
  // 0 0 0
  {
    block[offset+1][0]=1;
    block[offset][1]=1;
    block[offset+1][1]=1;
    block[offset+2][1]=1;       
  }        

  if (blocktype == 6)
  // 0 0
  //   0 0
  {
    block[offset][0]=1;
    block[offset+1][0]=1;
    block[offset+1][1]=1;
    block[offset+2][1]=1;         
  }    

  blockrotation = 0;
}



//**********************************************************************************************************************************************************  
boolean space_below()
{ 
  int i;
  int j;  
  for (i=rows-1;i>=0;i--)
  {
    for (j=0;j<cols;j++)
    {
       if (block[j][i])
       {
         if (i == rows-1)
           return false;
         if (pile[j][i+1])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}



//**********************************************************************************************************************************************************  
boolean space_left2()
{ 
  int i;
  int j;  
  for (i=rows-1;i>=0;i--)
  {
    for (j=0;j<cols;j++)
    {
       if (block[j][i])
       {
         if (j == 0 || j == 1)
           return false;
         if (pile[j-1][i] | pile[j-2][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}



//**********************************************************************************************************************************************************  
boolean space_left3()
{ 
  int i;
  int j;  
  for (i=rows-1;i>=0;i--)
  {
    for (j=0;j<cols;j++)
    {
       if (block[j][i])
       {
         if (j == 0 || j == 1 ||j == 2 )
           return false;
         if (pile[j-1][i] | pile[j-2][i]|pile[j-3][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}



//**********************************************************************************************************************************************************  
boolean space_left()
{ 
  int i;
  int j;  
  for (i=rows-1;i>=0;i--)
  {
    for (j=0;j<cols;j++)
    {
       if (block[j][i])
       {
         if (j == 0)
           return false;
         if (pile[j-1][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}



//**********************************************************************************************************************************************************  
boolean space_right()
{ 
  int i;
  int j;  
  for (i=rows-1;i>=0;i--)
  {
    for (j=0;j<cols;j++)
    {
       if (block[j][i])
       {
         if (j == cols-1)
           return false;
         if (pile[j+1][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}



//**********************************************************************************************************************************************************  
boolean space_right3()
{ 
  int i;
  int j;  
  for (i=rows-1;i>=0;i--)
  {
    for (j=0;j<cols;j++)
    {
       if (block[j][i])
       {
         if (j == cols-1||j == cols-2||j == cols-3)
           return false;
         if (pile[j+1][i] |pile[j+2][i] | pile[j+3][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}



//**********************************************************************************************************************************************************  
boolean space_right2()
{ 
  int i;
  int j;  
  for (i=rows-1;i>=0;i--)
  {
    for (j=0;j<cols;j++)
    {
       if (block[j][i])
       {
         if (j == cols-1 || j == cols-2)
           return false;
         if (pile[j+1][i] |pile[j+2][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}


#if 0
//**********************************************************************************************************************************************************  
void MatrixRefresh()
{
    int i;
    int k;

    boolean tmpdispUpper[8][8];
    boolean tmpdispLower[8][8];
     
    boolean tmppileUpper[8][8];
    boolean tmppileLower[8][8];
     
  
    //rotate 90 degrees for upper Bicolor LED matrix
    for (k=0;k<8;k++)
    {
      for(i=0;i<8;i++)
      {
        tmpdispUpper[k][i]=disp[i][k];
      }
    }  
  
  
    //rotate 90 degrees for lower Bicolor LED matrix
    for (k=8;k<16;k++)
    {
      for(i=0;i<8;i++)
      {
        tmpdispLower[k-8][i]=disp[i][k];
      }
    }  



    //For pile
    //rotate 90 degrees for upper Bicolor LED matrix
    for (k=0;k<8;k++)
    {
      for(i=0;i<8;i++)
      {
        tmppileUpper[k][i]=pile[i][k];
      }
    }  
  
  
    //rotate 90 degrees for lower Bicolor LED matrix
    for (k=8;k<16;k++)
    {
      for(i=0;i<8;i++)
      {
        tmppileLower[k-8][i]=pile[i][k];
      }
    }  

  

    for(i=0;i<8;i++)
    {      
       byte upper = 0;
       int b;
       for(b = 0;b<8;b++)
       {
         upper <<= 1;
         if (tmpdispUpper[b][i]) upper |= 1;
       }
       
       
       byte lower = 0;
       for(b = 0;b<8;b++)
       {
         lower <<= 1;
         if (tmpdispLower[b][i]) lower |= 1;
       }

            
      if (gameoverFlag == true)
      {  
        elapsedTime = millis() - startTime;

        // Display random pattern for pre-defined period before blanking display
        if (elapsedTime < 2000)
        {            
     
          cnt = cnt + 1;
          
          if (cnt > 80)
          {
            cnt = 0;
          }
        }   
      
      }
    } 
    
    
    
    if (gameoverFlag == false)
    {  
      // For pile - to display orange    
      for(i=0;i<8;i++)
      {      
         byte upper = 0;
         int b;
         for(b = 0;b<8;b++)
         {
           upper <<= 1;
           if (tmppileUpper[b][i]) upper |= 1;
         }
       
       
         byte lower = 0;
         for(b = 0;b<8;b++)
         {
           lower <<= 1;
           if (tmppileLower[b][i]) lower |= 1;
         }
      
      }         
    }    
}
#endif
