/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU CC; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * 
 * Copyright (C) 2002 Vino Crescini
 *
 */

import java.awt.*;
import java.awt.event.*;

public class vcalcpanel extends Panel implements ActionListener {
  private Button b0 = new Button("0");
  private Button b1 = new Button("1");
  private Button b2 = new Button("2");
  private Button b3 = new Button("3");
  private Button b4 = new Button("4"); 
  private Button b5 = new Button("5");
  private Button b6 = new Button("6");
  private Button b7 = new Button("7");
  private Button b8 = new Button("8");
  private Button b9 = new Button("9");
  private Button bdot = new Button(".");
  private Button beq = new Button("=");
  private Button bplus = new Button("+");
  private Button bminus = new Button("-");
  private Button btimes = new Button("*");
  private Button bdivide = new Button("/");
  private Button bmemadd = new Button("M+");
  private Button bmemrem = new Button("M-");
  private Button bmemrec = new Button("MR");
  private Button bcancel = new Button("C");
  private vcalcdisplay cdisplay = new vcalcdisplay();
  
  public vcalcpanel() {
    Panel pkeypad = new Panel(new GridLayout(5, 4));

    b0.addActionListener(this);
    b1.addActionListener(this);
    b2.addActionListener(this);
    b3.addActionListener(this);
    b4.addActionListener(this);
    b5.addActionListener(this);
    b6.addActionListener(this);
    b7.addActionListener(this);
    b8.addActionListener(this);
    b9.addActionListener(this);
    bplus.addActionListener(this);
    bminus.addActionListener(this);
    btimes.addActionListener(this);
    bdivide.addActionListener(this);
    beq.addActionListener(this);
    bdot.addActionListener(this);
    bmemadd.addActionListener(this);
    bmemrem.addActionListener(this);
    bmemrec.addActionListener(this);
    bcancel.addActionListener(this);

    pkeypad.add(bcancel);
    pkeypad.add(bmemadd);
    pkeypad.add(bmemrem);
    pkeypad.add(bmemrec);
    pkeypad.add(b7); 
    pkeypad.add(b8); 
    pkeypad.add(b9); 
    pkeypad.add(btimes);
    pkeypad.add(b4); 
    pkeypad.add(b5); 
    pkeypad.add(b6); 
    pkeypad.add(bdivide);
    pkeypad.add(b1); 
    pkeypad.add(b2); 
    pkeypad.add(b3);
    pkeypad.add(bplus);
    pkeypad.add(b0);
    pkeypad.add(bdot);
    pkeypad.add(beq);
    pkeypad.add(bminus);

    setLayout(new BorderLayout());
    add(cdisplay, "North");
    add(pkeypad, "Center");
  }

  public void actionPerformed(ActionEvent e) {
    switch(e.getActionCommand().charAt(0)) {
      case '0' : 
      case '1' : 
      case '2' : 
      case '3' : 
      case '4' : 
      case '5' : 
      case '6' : 
      case '7' : 
      case '8' : 
      case '9' : 
        cdisplay.pressed_num(Integer.parseInt(e.getActionCommand()));
        break;
      case '+' :
      case '-' :
      case '*' :
      case '/' :
        cdisplay.pressed_op(e.getActionCommand().charAt(0));
        break; 
      case '=' :
        cdisplay.pressed_equals(); 
        break; 
      case 'C' :
        cdisplay.pressed_c();
        break;
    }
  }
}
