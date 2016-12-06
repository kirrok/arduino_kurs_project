import processing.serial.*;
import java.util.Stack;
import java.util.stream.Collectors;

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
  
  table.fill("СКОРОСТЬ", serHelper.getSp());
  serHelper.drop("СКОРОСТЬ");
  table.fill("РАССТОЯНИЕ", serHelper.getDist());
  serHelper.drop("РАССТОЯНИЕ");
  table.fill("НАПРАВЛЕНИЕ", serHelper.getDir());
  serHelper.drop("СКОРОСТЬ");
  delay(600);
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

  public void fill(String label, Stack<String> values) {  
     getColumn(label).getValues().addAll(values);
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
  
  public ArrayList<String> getValues() {
    return values;
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
    
    if (values.size() != 0) {
      for(String value: values.subList(Math.max(values.size() - 10, 0), values.size())) {
        text(value, width /2 + x, y + labelHeight + row * rowHeight);
        row++;  
      }
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
  
  public void drop(String bufName) {
    switch(bufName) {
      case "РАССТОЯНИЕ" : 
        disS.clear();
        break;
      case "СКОРОСТЬ" : 
        spdS.clear();
        break;
      case "НАПРАВЛЕНИЕ" : 
        dirS.clear();
        break;
    }
  }
  
  public Stack<String> getDir() {
    return dirS;
  }
  public Stack<String> getSp() {
    return spdS;
  }
  public Stack<String> getDist() {
    return disS;
  }
  
  public void read() {
    if(port.available() > 0) {
      inputType = port.readChar();
    } 
    
    String curInput = Integer.toString(port.read());
    
    if(curInput != null) {
     switch(inputType) {
       case speed: spdS.push(curInput);
       break;
       case distance: disS.push(curInput);
       break;
       case direction: dirS.push(curInput);
       break;
     }
    }
    
    if(port.available() > 0) {
      this.read();
    } 
  }  
}