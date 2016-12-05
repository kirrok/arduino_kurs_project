import processing.serial.*;
import java.util.Stack;
Serial bluetoothPort;
SerialHelper serHelper;

Table table;
static int k = 0;

void setup() {

  final String expectedPort = Serial.list()[0];
  
  try {
    println("Пробуем подключиться к : " + expectedPort);
    bluetoothPort = new Serial(this, expectedPort, 9600);
    println("Есть контакт!");
  } 
  catch (Exception e) {
    println("Не удалось подключиться. Порт занят");
  }
  serHelper = new SerialHelper(bluetoothPort);
  //printArray(Serial.list());

  size (600, 600);
  background(0);

  table = new Table(600, 400);

  table.addColumn(new Column("СКОРОСТЬ"));
  table.addColumn(new Column("РАССТОЯНИЕ"));
  table.addColumn(new Column("НАПРАВЛЕНИЕ"));
}

void draw() {
  background(0);
  
  serHelper.read();

  table.draw();
  
  table.getColumn("СКОРОСТЬ").addValue("" + k++);
  table.getColumn("РАССТОЯНИЕ").addValue("" + k++);
  table.getColumn("НАПРАВЛЕНИЕ").addValue("" + k++);
  
  delay(150);
}

void keyPressed() {
  if(key == 'w' || key == 's' || key == 'a' || key == 'd') {
    bluetoothPort.write(key);
  }
}

private class Table {
  private int width;
  private int height;
  private ArrayList<Column> columns = new ArrayList<Column>();
  private boolean sizesSet = false;

  Table(int width, int height) {
    this.width = width;
    this.height = height;
  }
  
  public Column getColumn(String label) {
      for (Column col : columns) {
        if (col.label == label) {
          return col;
        }
      }
      return null;
  }
  public void addColumn(Column column) {
    columns.add(column);
    sizesSet = false;
  }

  public void draw() {
    if (!sizesSet) {
      setColumnSize();
    }
    for ( Column col : columns) {
      col.draw();
    }
  }

  private void setColumnSize() {
    int columnCount = columns.size();

    int interval = int(width * 0.02);

    int topIndent = int(height * 0.05);

    int columnWidth = int(( this.width - (columnCount + 1) * interval ) / columnCount);

    int columnHeight = int(height * 0.7); 


    int x = interval;
    int y = topIndent;
    int horizontalDiff = interval + columnWidth;


    for (Column column : columns) {
      column.setSize(x, y, columnWidth, columnHeight);
      x += horizontalDiff;
    }
    sizesSet = true;
  }
}

private class Column {
  public String label;
  private final ArrayList<String> values = new ArrayList<String>();

  private int labelTextSize = 16;
  private int labelHeight = 30;
  private int rowHeight = 20;
  private int dataSize = 16; 
  
  private color backgroundColor = 150;

  private int x;
  private int y;
  private int height;
  private int width;

  Column(String label) {
    this.label = label;
  }
  
  public void setColor(int col) {
    backgroundColor = col;
  }
  
  public void addValue(String currentValue) {
    values.add(currentValue);
  }

  public void setSize(int x, int y, int w, int h) {
    this.x = x;
    this.y = y;
    this.height = h;
    this.width = w;
  }
  
  public void draw() {
    stroke(255, 50, 50);
    noFill();
    rect(x, y, width, height);
    
    line(x, y + labelHeight, x + width, y + labelHeight);
    textAlign(CENTER, TOP);
    textSize(labelTextSize);
    text(label, width /2 + x, y);
    textSize(dataSize);
    
    int row = 1;
    
    for(String value: values.subList(Math.max(values.size() - 10, 0), values.size())) {
      text(value, width /2 + x, y + labelHeight + row * rowHeight);
      row++;  
    }
  }
}


private class SerialHelper {
  private Serial port;
  
  private final char direction = 'r'; 
  private final char speed = 's';
  private final char distance = 'd';
  
  private Stack<String> dirS = new Stack<String>();
  private Stack<String> spdS = new Stack<String>();
  private Stack<String> disS = new Stack<String>();
  
  private char inputType;
  
  SerialHelper(Serial port) {
    this.port = port;
  }
  
  public void read() {
    if(port.available() > 0) {
      inputType = port.readChar();
      println(inputType);
    } 
    
     switch(inputType) {
       case speed: spdS.push(port.readString());
       break;
       case distance: disS.push(port.readString());
       break;
       case direction: dirS.push(port.readString());
       break;
     }
  }  
}