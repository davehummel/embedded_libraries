#ifndef DH_CONTROLLEDTOUCHLCD_H__
#define DH_CONTROLLEDTOUCHLCD_H__
#include "dh_controller.h"
#include "SPI.h"
#include "ILI9341_t3.h"
#include <XPT2046_Touchscreen.h>
#include "CTFont.h"

//#define DEBUG ON

#define TCH_CS  8
//#define TCH_CS  17

#define TFT_DC      9
//#define TFT_DC      20

#define TFT_CS      10
//#define TFT_CS      15
#define TFT_RST    255  // 255 = unused, connect to 3.3V
#define TFT_MOSI     11
#define TFT_SCLK    14
#define TFT_MISO    12
#define TFT_PWM 255

#define MAX_DRAW 100
#define MAX_TEXT_LEN 64

#define CLEAR_WIDTH 5
#define CLEAR_COUNT 64

#define TS_MINX 350
#define TS_MINY 200
#define TS_MAXX 3700
#define TS_MAXY 3800

#define REPRESS_TIME 200

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

class BoundNum {
public:

		bool parse(uint16_t &pointer,char* text, Controller* controller){
	 	if (text[pointer] == '$'){
			pointer++;
			ADDR1 temp(pointer,text);
			addr1 = temp;
			if (text[pointer]=='.'){
				pointer++;
				if (!Controller::parse_uint8(addr2,pointer,text)){
					  	controller->getErrorLogger()->setParseError(text,pointer,"Expected addr2 number");
							controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
					return false;
				}
			}else{
				addr2 = 0;
			}


			module = controller->getControlled(addr1.modID);


			if (module == 0 ){
				controller->getErrorLogger()->print("Unknown module:");
				controller->getErrorLogger()->println(addr1.modID);
				controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
				return false;
			}
			#ifdef DEBUG
			Serial.println("Parsed numeric variable");
			#endif
			return true;
		}else{
			if (!Controller::parse_uint16(val,pointer,text)){
				controller->getErrorLogger()->setParseError(text,pointer,"Unable to parse number");
				controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
				return false;
			}
			return true;
		}

	}

	bool update(){
		if (module == 0)
			return false;
		uint16_t temp = 0;
		switch ( addr1.type){
			case A_BYTE: temp = (module->readB(addr1,addr2)); break;
			case A_UINT: temp = (module->readU(addr1,addr2)); break;
			case A_INT: temp = (module->readI(addr1,addr2)); break;
			case A_LONG: temp = (module->readL(addr1,addr2)); break;
			case A_FLOAT: temp = (module->readF(addr1,addr2)); break;
			case A_DOUBLE: temp = (module->readD(addr1,addr2)); break;
			case A_TIME: temp = (module->readT(addr1,addr2)); break;
			case A_STRING: temp = 0; break;
		}

		if (temp != val){
			val = temp;
			return true;
		}
		return false;
	}

	uint16_t getVal(){
		return val;
	}

	Controller::Controlled* module=0;
	ADDR1 addr1;
	uint8_t addr2 = 0;

	uint16_t val;
};

class BoundText: public Print {
public:
	bool parse(uint16_t &pointer,char* text, Controller* controller){
		constText[0] = '\0';
		if (text[pointer]=='"'){

			pointer++;
			int i;
			for (i = 0; i < MAX_TEXT_LEN ; i++){

				if (i == MAX_TEXT_LEN - 1){
					constText[i] = '\0';
				}else{
					constText[i] = text[pointer+i];
					if (constText[i]=='"'){
						constText[i] = '\0';
						break;
					}
				}
			}
			#ifdef DEBUG
			Serial.print("Parsed text:");
			Serial.println(constText);
			#endif
			pointer+=i+1;
			module = 0;
			addr2 = 0;
			return true;
		}else if (text[pointer] == '$'){
			pointer++;
			ADDR1 temp(pointer,text);
			addr1 = temp;
			if (text[pointer]=='.'){
				pointer++;
				if (!Controller::parse_uint8(addr2,pointer,text)){
					controller->getErrorLogger()->setParseError(text,pointer,"Expected addr2 number");
					controller->getErrorLogger()->finished(Controller::lastProcessedMSTime,ErrorLogger::MOD_PARSER);
					return false;
				}
			}else{
				addr2 = 0;
			}


			module = controller->getControlled(addr1.modID);


			if (module == 0 ){
				Serial.print("Failed to find controlled module:");
				Serial.println(addr1.modID);
				return false;
			}
			#ifdef DEBUG
			Serial.println("Parsed text variable");
			#endif
			return true;
		}
		Serial.println("Requires either \" or $");
		return false;
	}

	bool update(){
		if (module == 0 )
			return false;
		compPointer = 0;
		changed = false;
		switch ( addr1.type){
			case A_BYTE: print(module->readB(addr1,addr2)); break;
			case A_UINT: print(module->readU(addr1,addr2)); break;
			case A_INT: print(module->readI(addr1,addr2)); break;
			case A_LONG: print(module->readL(addr1,addr2)); break;
			case A_FLOAT: print(module->readF(addr1,addr2)); break;
			case A_DOUBLE: print(module->readD(addr1,addr2)); break;
			case A_TIME: print(module->readT(addr1,addr2)); break;
			case A_STRING: print(module->readS(addr1,addr2)); break;
			case BAD_TYPE: print("BAD_TYPE"); break;
		}


		constText[compPointer]='\0';

		return changed;

	}

	size_t write(uint8_t c){
		changed |= (constText[compPointer]!=c);
		constText[compPointer] = c;
		compPointer++;
		return 1;
	}

	void printVal(ILI9341_t3 *tft){
			tft->print(constText);
			return;

	}

	virtual ~BoundText(){
	}

	Controller::Controlled* module;
	ADDR1 addr1;
	uint8_t addr2 = 0;

	char constText[64] ;
	bool changed = false;
	uint8_t compPointer = 0;

};

class Drawable{
	public:
	virtual bool parse(uint16_t &pointer,char* text, Controller* controller)=0;
	virtual void draw(ILI9341_t3 *tft) = 0;
	bool update(){
		if (firstDraw){
			_update();
			firstDraw = false;
			return true;
		}
		return _update();
	}
	virtual bool _update()=0;
	virtual bool inBounds(uint16_t x, uint16_t y) {
		return false;
	};

	virtual ~Drawable(){};

	uint16_t x = 0;
	uint16_t y = 0;

	bool firstDraw = true;

  bool mappedFunc = false;

	ADDR1 funcAddr1;
	uint8_t funcAddr2 = 0;

};

class TextDrawable: public Drawable{
public:
	void setButton(bool isB){
		isButton = isB;
	}

	bool parse(uint16_t &pointer,char* text,Controller* controller){
		if (isButton){
			if (!Controller::parse_uint16(bWidth,pointer,text)){
				Serial.println("Couldnt read button width");
				return false;
			}
			pointer++;
			if (!Controller::parse_uint16(bHeight,pointer,text)){
				Serial.println("Couldnt read button height");
				return false;
			}
			pointer++;
		}

		if (text[pointer] == 'B'){
			bold = true;
			pointer++;
		}

		if (!Controller::parse_uint8(size,pointer,text)){
			Serial.println("Couldnt read font size");
			return false;
		}
		switch(size){
			case 8: font = &(bold?LiberationSans_8_Bold:LiberationSans_8);break;
			case 9: font = &(bold?LiberationSans_9_Bold:LiberationSans_9);break;
			case 10: font = &(bold?LiberationSans_10_Bold:LiberationSans_10);break;
			case 11: font = &(bold?LiberationSans_11_Bold:LiberationSans_11);break;
			case 12: font = &(bold?LiberationSans_12_Bold:LiberationSans_12);break;
			case 13: font = &(bold?LiberationSans_13_Bold:LiberationSans_13);break;
			case 14: font = &(bold?LiberationSans_14_Bold:LiberationSans_14);break;
			case 16: font = &(bold?LiberationSans_16_Bold:LiberationSans_16);break;
			case 18: font = &(bold?LiberationSans_18_Bold:LiberationSans_18);break;
			case 20: font = &(bold?LiberationSans_20_Bold:LiberationSans_20);break;
			case 24: font = &(bold?LiberationSans_24_Bold:LiberationSans_24);break;
			default:
			Serial.println("Bad Font size.  Use 8,9,10,11,12,13,14,16,18,20,24");
			return false;
		}

		pointer++;

		if (!label.parse(pointer,text,controller)){
			Serial.println("Text could not read text component");
			return false;
		}

		pointer++;

		if (!fgR.parse(pointer,text,controller)){
			Serial.println("Text could not read red foreground component");
			return false;
		}
				pointer++;

		if (!fgG.parse(pointer,text,controller)){
			Serial.println("Text could not read green foreground component");
			return false;
		}
				pointer++;

		if (!fgB.parse(pointer,text,controller)){
			Serial.println("Text could not read blue foreground component");
			return false;
		}
				pointer++;

		if (!bgR.parse(pointer,text,controller)){
			Serial.println("Text could not read red background component");
			return false;
		}
				pointer++;

		if (!bgG.parse(pointer,text,controller)){
			Serial.println("Text could not read green background component");
			return false;
		}
				pointer++;

		if (!bgB.parse(pointer,text,controller)){
			Serial.println("Text could not read blue background component");
			return false;
		}



		return true;
	}
	void draw(ILI9341_t3 *tft){
		tft->setFont(*font);
		tft->setTextColor(fg);
			if (isButton){
				tft->fillRect(x, y, bWidth, bHeight, bg);
				tft->drawRect(x, y, bWidth, bHeight, fg);
				tft->setCursor(x+2,y+(bHeight-size)/3);
				label.printVal(tft);
			} else {
				if (lastW>0){
					tft->fillRect(x, y, lastW, size+size/4, bg);
				}

				tft->setCursor(x,y);
				#ifdef DEBUG
				Serial.print("Printing label:");
				Serial.println(label.constText);
				#endif
				label.printVal(tft);
				lastW = tft->getCursorX()-x;
				if (endDot){
					tft->drawPixel(lastW+x,y,fg);
				}
			}

	}
	bool _update(){
		bool changed = false;
		changed |=fgR.update();
		changed |=fgG.update();
		changed |=fgB.update();

		changed |=bgR.update();
		changed |=bgG.update();
		changed |=bgB.update();


			fg = color565((uint8_t)fgR.getVal(),(uint8_t)fgG.getVal(),(uint8_t)fgB.getVal());
			bg = color565((uint8_t)bgR.getVal(),(uint8_t)bgG.getVal(),(uint8_t)bgB.getVal());



		changed |=label.update();
		return changed;

	}

	bool inBounds(uint16_t _x, uint16_t _y) {
		if (isButton){
			if (_x>x && _x-x < bWidth){
				if (_y>y && _y-y < bHeight)
					return true;
			}
		} else{
			if (_x>x && _x-x < lastW){
				if (_y>y && _y-y < size + size / 4)
					return true;
			}
		}
		return false;
	};

	~TextDrawable(){

	}
private:
	bool bold = false;
	bool endDot = true;
	uint8_t size = 0;
	const ILI9341_t3_font_t *font = 0;
	BoundText label ;
	BoundNum fgR,fgG, fgB ;
	BoundNum bgR , bgG , bgB;
	uint16_t fg=0,bg=0;
	uint16_t lastW = 0;
	uint16_t bWidth,bHeight;
	bool isButton= false;
};

class RectDrawable: public Drawable{
public:
	bool parse(uint16_t &pointer,char* text,Controller* controller){

		if (text[pointer] == 'R'){
			pointer++;
			if (!Controller::parse_uint8(rounded,pointer,text)){
				Serial.println("Couldnt read rounded");
				return false;
			}
			pointer++;
		}

		if (!Controller::parse_uint16(w,pointer,text)){
			Serial.println("Couldnt read width");
			return false;
		}

		pointer++;

		if (!Controller::parse_uint16(h,pointer,text)){
			Serial.println("Couldnt read height");
			return false;
		}

		pointer++;

		if (!fgR.parse(pointer,text,controller)){
			Serial.println("Text could not read red foreground component");
			return false;
		}
				pointer++;

		if (!fgG.parse(pointer,text,controller)){
			Serial.println("Text could not read green foreground component");
			return false;
		}
				pointer++;

		if (!fgB.parse(pointer,text,controller)){
			Serial.println("Text could not read blue foreground component");
			return false;
		}
				pointer++;

		if (!bgR.parse(pointer,text,controller)){
			Serial.println("Text could not read red background component");
			return false;
		}
				pointer++;

		if (!bgG.parse(pointer,text,controller)){
			Serial.println("Text could not read green background component");
			return false;
		}
				pointer++;

		if (!bgB.parse(pointer,text,controller)){
			Serial.println("Text could not read blue background component");
			return false;
		}



		return true;
	}
	void draw(ILI9341_t3 *tft){
		if (rounded==0){
			tft->fillRect(x, y, w, h, bg);

			if (bg!=fg ){

				tft->drawRect(x, y, w, h, fg);
			}
		}else{

			tft->fillRoundRect(x, y, w, h, rounded, bg);

			if (bg!=fg ){

				tft->drawRoundRect(x, y, w, h, rounded, fg);
			}
		}

	}
	bool _update(){
		bool changed = false;

		changed |=fgR.update();
		changed |=fgG.update();
		changed |=fgB.update();

		changed |=bgR.update();
		changed |=bgG.update();
		changed |=bgB.update();



			fg = color565((uint8_t)fgR.getVal(),(uint8_t)fgG.getVal(),(uint8_t)fgB.getVal());
			bg = color565((uint8_t)bgR.getVal(),(uint8_t)bgG.getVal(),(uint8_t)bgB.getVal());


		return changed;

	}

	bool inBounds(uint16_t _x, uint16_t _y) {

			if (_x>x && _x-x < w){
				if (_y>y && _y-y < h)
					return true;
			}

		return false;
	};


private:
	uint8_t rounded = 0;

	BoundNum fgR,fgG, fgB ;
	BoundNum bgR , bgG , bgB;
	uint16_t fg=0,bg=0;
	uint16_t w,h = 0;

};

class CircleDrawable: public Drawable{
public:
	bool parse(uint16_t &pointer,char* text,Controller* controller){


		if (!Controller::parse_uint16(w,pointer,text)){
			Serial.println("Couldnt read radius");
			return false;
		}

		pointer++;

		if (!fgR.parse(pointer,text,controller)){
			Serial.println("Text could not read red foreground component");
			return false;
		}
				pointer++;

		if (!fgG.parse(pointer,text,controller)){
			Serial.println("Text could not read green foreground component");
			return false;
		}
				pointer++;

		if (!fgB.parse(pointer,text,controller)){
			Serial.println("Text could not read blue foreground component");
			return false;
		}
				pointer++;

		if (!bgR.parse(pointer,text,controller)){
			Serial.println("Text could not read red background component");
			return false;
		}
				pointer++;

		if (!bgG.parse(pointer,text,controller)){
			Serial.println("Text could not read green background component");
			return false;
		}
				pointer++;

		if (!bgB.parse(pointer,text,controller)){
			Serial.println("Text could not read blue background component");
			return false;
		}



		return true;
	}
	void draw(ILI9341_t3 *tft){

			tft->fillCircle(x, y, w, bg);

			if (bg!=fg ){

				tft->drawCircle(x, y, w, fg);
			}

	}
	bool _update(){
		bool changed = false;

		changed |=fgR.update();
		changed |=fgG.update();
		changed |=fgB.update();

		changed |=bgR.update();
		changed |=bgG.update();
		changed |=bgB.update();



			fg = color565((uint8_t)fgR.getVal(),(uint8_t)fgG.getVal(),(uint8_t)fgB.getVal());
			bg = color565((uint8_t)bgR.getVal(),(uint8_t)bgG.getVal(),(uint8_t)bgB.getVal());


		return changed;

	}

private:

	BoundNum fgR,fgG, fgB ;
	BoundNum bgR , bgG , bgB;
	uint16_t fg=0,bg=0;
	uint16_t w = 0;

};

class LineDrawable: public Drawable{
public:
	bool parse(uint16_t &pointer,char* text,Controller* controller){

		if (text[pointer] == 'X'){
			horiz = true;
			pointer++;
		} else if (text[pointer] == 'Y'){
			horiz = false;
			pointer++;
		} else {
			Serial.print("L must be followed by X or Y for horiz or vert lines : ");
			Serial.println(text[pointer]);
			return false;
		}

		if (!Controller::parse_uint16(w,pointer,text)){
			Serial.println("Couldnt read radius");
			return false;
		}

		pointer++;

		if (!fgR.parse(pointer,text,controller)){
			Serial.println("Text could not read red foreground component");
			return false;
		}
				pointer++;

		if (!fgG.parse(pointer,text,controller)){
			Serial.println("Text could not read green foreground component");
			return false;
		}
				pointer++;

		if (!fgB.parse(pointer,text,controller)){
			Serial.println("Text could not read blue foreground component");
			return false;
		}

		return true;
	}
	void draw(ILI9341_t3 *tft){


			if (horiz){
				tft->drawFastHLine(x,y,w,fg);
			}else{
				tft->drawFastVLine(x,y,w,fg);
			}

	}
	bool _update(){
		bool changed = false;

		changed |=fgR.update();
		changed |=fgG.update();
		changed |=fgB.update();

			fg = color565((uint8_t)fgR.getVal(),(uint8_t)fgG.getVal(),(uint8_t)fgB.getVal());


		return changed;

	}

private:

	BoundNum fgR,fgG, fgB ;
	uint16_t fg=0;
	uint16_t w = 0;
	bool horiz;

};

class ControlledTouchLCD: public Controller::Controlled{
public:

	char* readS(ADDR1 addr,uint8_t addr2){
		return 0;
	}

	void write(ADDR1 addr,uint8_t val){

		switch (addr.addr%26){
			case 1:
				analogWrite(TFT_PWM,val);
				break;
			case 17:
				tft->setRotation(val);
				break;
		}
	}

	void write(ADDR1 addr,uint16_t val){
		bgColor = val;
	}

	int16_t readI(ADDR1 addr,uint8_t addr2){
		if (addr2 == 0)
			return touchX;
		else if (addr2 == 1)
			return touchY;
		return 0;
	}

	uint8_t readB(ADDR1 addr,uint8_t addr2){
		return lastButtonID;
	}


	uint32_t readT(ADDR1 addr,uint8_t addr2){
		return lastButtonTime;
	}




	void begin(void){
		tft = new ILI9341_t3(tft_cs, tft_dc, tft_rst, tft_mosi, tft_sclk, tft_miso);
		ts = new XPT2046_Touchscreen(tch_cs);
		pinMode(TFT_PWM,OUTPUT);
		digitalWrite(TFT_PWM,LOW);
		tft->begin();
		ts->begin();
		tft->setTextWrap(false);
	}

	uint8_t tft_cs=TFT_CS,tft_dc=TFT_DC,tft_rst=TFT_RST,tft_mosi=TFT_MOSI,tft_sclk=TFT_SCLK,tft_miso=TFT_MISO,tch_cs=TCH_CS;

	void execute(uint32_t _time,uint32_t id,char command[]){
		if (command[0] == 'f' || command[0] == 'F'){
			#ifdef DEBUG
			Serial.println("Mapping function!");
			#endif
			uint16_t pointer = 4;
			uint8_t zID;
			if (!Controller::parse_uint8(zID,pointer,command) ){
				controller->getErrorLogger()->println("Unable to parse zID:");
				controller->getErrorLogger()->finished(_time,ErrorLogger::MOD_PARSER);
				return ;
			}
			if (zID > MAX_DRAW){
			controller->getErrorLogger()->print("zID is too large:");
				controller->getErrorLogger()->println(zID);
				controller->getErrorLogger()->finished(_time,ErrorLogger::MOD_PARSER);
				return ;
			}

			if (items[zID]== 0){
				controller->getErrorLogger()->print("No drawble for id:");
				controller->getErrorLogger()->println(zID);
				controller->getErrorLogger()->finished(_time,ErrorLogger::MOD_PARSER);
				return;
			}

			items[zID]->mappedFunc = false;

			if (command[pointer]=='\0'){
				return;
			}

			pointer++;

			items[zID]->funcAddr1 = ADDR1(pointer,command);
			pointer++;

			if (!Controller::parse_uint8(items[zID]->funcAddr2,pointer,command) ){
				controller->getErrorLogger()->println("Unable to parse Addr2:");
				controller->getErrorLogger()->finished(_time,ErrorLogger::MOD_PARSER);
				return ;
			}

			items[zID]->mappedFunc = true;
			return;
		}

		if (command[0] == 'c' || command[0] == 'C'){
			#ifdef DEBUG
			Serial.println("Clearing!");
			#endif
			if (command[1] == 'a' || command[1] == 'A'){
				#ifdef DEBUG
					Serial.println("And removing renderables!");
				#endif
					for (uint8_t i = 0; i < MAX_DRAW ; i++){
						if (items[i] != 0 ){
							delete items[i];
							items[i] = 0;
						}
					}
			} else {
					for (uint8_t i = 0; i < MAX_DRAW ; i++){
						if (items[i] != 0 ){
							items[i]->firstDraw = true;
						}
					}
			}
			clearCycle = 0;
			return;
		}
		if (command[0] == 'd' || command[0] == 'D'){
			#ifdef DEBUG
			Serial.println("Drawing!");
			#endif
			boolean istouched = ts->touched();
			if (istouched) {
			    TS_Point p = ts->getPoint();

  				 switch(tft->getRotation()){
  				 	case 0:{
  				 		uint16_t temp = p.x;
  				 		p.x = p.y;
  				 		p.y = temp;
  				 		p.x = map(p.x, TS_MINX, TS_MAXX,  tft->width(),0);
  				 		p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft->height());
  				 		break;
  				 	}
  				 	case 1:{
  				 		p.x = map(p.x, TS_MINX, TS_MAXX,  0,tft->width());
  				 		p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft->height());
  				 		break;
  				 	}
  				 	case 2:{
  				 		uint16_t temp = p.x;
  				 		p.x = p.y;
  				 		p.y = temp;
  				 		p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft->width());
  				 		p.y = map(p.y, TS_MINY, TS_MAXY,  tft->height(),0);
  				 		break;
  				 	}
  				 	 case 3:{
  				 		p.x = map(p.x, TS_MINX, TS_MAXX,  tft->width(),0);
  				 		p.y = map(p.y, TS_MINY, TS_MAXY, tft->height(),0);
  				 		break;
  				 	}
  				 }
  				touchX = p.x;
  				touchY = p.y;
			    draw(true,_time);
			}else{
				 touchX = -1;
  				touchY = -1;
				draw(false,_time);
			}

			return;
		}

		if (command[0]>= '0' && command[0] <= '9'){
			#ifdef DEBUG
			Serial.println("Adding item!");
			#endif
			uint8_t zID = 0;
			uint16_t x = 0;
			uint16_t y = 0;

			uint16_t pointer = 0;

			if (!Controller::parse_uint8(zID,pointer,command) ){
				Serial.println("Unable to parse zID number first");
				return ;
			}
			if (zID > MAX_DRAW){
				Serial.println("zID is to large");
				return ;
			}
			if (items[zID]!=0){
				#ifdef DEBUG
				Serial.print("Deleted item :");
				Serial.println(zID);
				#endif
				delete items[zID];
				items[zID]  = 0;
			}
			if (command[pointer]=='\0')
				return;
			pointer++;
			if (!Controller::parse_uint16(x,pointer,command) ){
				Serial.println("Unable to parse x position");
				return ;
			}
			pointer++;
			if (!Controller::parse_uint16(y,pointer,command) ){
				Serial.println("Unable to parse y position");
				return ;
			}
			pointer++;
			switch (command[pointer]){
				case 'T': items[zID]  =  new TextDrawable(); break;
				case 'B': items[zID]  =  new TextDrawable();((TextDrawable*)items[zID])->setButton(true); break;
				case 'R': items[zID]  =  new RectDrawable(); break;
				case 'C': items[zID]  =  new CircleDrawable(); break;
				case 'L': items[zID]  =  new LineDrawable(); break;
				default: Serial.println("Unknown renderable type"); return;
			}
			items[zID] -> x = x;
			items[zID] -> y = y;
			pointer+=2;
			if (!items[zID]->parse(pointer,command,controller)){
				Serial.println("Failed to parse renderable");
				delete items[zID];
				items[zID] = 0;
				return;
			}
			return;
		}
		Serial.println("Failed to process Display request");

	}

	void startSchedule(char command[], uint32_t id){

	}

	void endSchedule(char command[], uint32_t id){

	}

private:

	void draw(bool isTouch, uint32_t _time){
		if (clearCycle<CLEAR_COUNT){
			#ifdef DEBUG
			Serial.println("Clearing background");
			#endif
			if (tft->width()>tft->height()){
				tft->fillRect(clearCycle*CLEAR_WIDTH,0,CLEAR_WIDTH,240,bgColor);
			}else{
				tft->fillRect(0,clearCycle*CLEAR_WIDTH,240,CLEAR_WIDTH,bgColor);
			}
			clearCycle++;
			return;
		}

		uint8_t step = drawCycle;
		while(true){

			if (items[step]!=0){
				#ifdef DEBUG
					Serial.print("Drawing ");
					Serial.println(step);
				#endif
				if (items[step]->update()){
					items[step]->draw(tft);
				}
				break;
			}else{
				step++;
				if (step == MAX_DRAW)
					step = 0;
			}
			if (step == drawCycle)
				break;

		}
		drawCycle=step+1;

		if (drawCycle==MAX_DRAW){
			drawCycle = 0;
		}

		if (isTouch){
				#ifdef DEBUG
			Serial.print("Try:");
			Serial.print(touchX);
			Serial.print(",");
			Serial.print(touchY);
			Serial.println();
			#endif
			for (uint8_t i = 0 ; i < MAX_DRAW ; i++){
				if (items[i]!=0){
					if (items[i]->inBounds(touchX,touchY)){
						bool repeat = lastButtonID==i;
						lastButtonID = i;
						lastButtonTime = _time;
						if (repeat && _time-lastButtonPermTime<REPRESS_TIME)
							continue;
						if (items[i]->mappedFunc){
							lastButtonPermTime = _time;
							 ADDR1 addr1 = items[i]->funcAddr1;
							 uint8_t addr2 = items[i]->funcAddr2;
							 Controlled* controlled = controller->getControlled(addr1.modID);
							 if (controlled!=0){
								 switch (addr1.type){
		 							case A_BYTE: controlled->readB(addr1,addr2);
		 							break;
		 							case A_UINT: controlled->readU(addr1,addr2);
		 							break;
		 							case A_INT: controlled->readI(addr1,addr2);
		 							break;
		 							case A_FLOAT: controlled->readF(addr1,addr2);
		 							break;
		 							case A_LONG: controlled->readL(addr1,addr2);
		 							break;
		 							case A_DOUBLE: controlled->readD(addr1,addr2);
		 							break;
		 							case A_TIME: controlled->readT(addr1,addr2);
		 							break;
		 							case A_STRING: controlled->readS(addr1,addr2);
									break;
									case BAD_TYPE: break;
		 						}
							 }
						}
					}
				}
			}
		}
	}

	ILI9341_t3 *tft;
	XPT2046_Touchscreen *ts;
	uint8_t pwmBrightness;
	uint16_t bgColor;

	uint8_t clearCycle = 0;
	uint8_t drawCycle = 0;

	int16_t touchX = -1,touchY = -1;

	uint8_t lastButtonID = 0;
	uint32_t lastButtonTime = 0;
	uint32_t lastButtonPermTime = 0;

	Drawable* items[MAX_DRAW]={};
};




#endif
