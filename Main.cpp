#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<time.h>
#include<math.h>
#include<allegro5/allegro.h>
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_image.h>
#include<allegro5/allegro_native_dialog.h>
#include<allegro5/allegro_ttf.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_acodec.h>
#include<allegro5/allegro_audio.h>
#include "items.h"



#define FLOOR 550
#define GRAVITY 1
#define JUMP_MAX_SPEED 26
#define JUMP_MAX_HEIGHT 600
#define TOTAL_BLOCK 10
#define TOTAL_GOLD 20 
#define w 930      
#define h 828.5




bool game = false;
bool play = true;
bool m = true;
bool sound = true;
int goldPos = 0;
int distance = 0;
int score = 0;
int highscore = 0;
int level = 1;
ALLEGRO_DISPLAY* screen;
ALLEGRO_FONT* font;
ALLEGRO_FONT* font2;


//prototypes
void menu();
void roadLoop(float* dx, float* dy, float* dw, float* dh);
void initBlock(struct item b[], int x);
void drawPassedBlock(struct item block[], int total);
void drawBlock(struct item block[], int total);
void createBlock(struct item block[], char t, int total);
void moveBlocks(struct item blocks[]);
void settings();
void playGame();

int collision(struct item player, struct item block[]);
void playSound(ALLEGRO_SAMPLE* sample);


enum keys { LEFT, RIGHT, UP, DOWN };


bool KEY[4] = { false , false , false , false };
ALLEGRO_BITMAP* imageRoad;
ALLEGRO_BITMAP* sun;
ALLEGRO_BITMAP* coin;
ALLEGRO_BITMAP* box;
ALLEGRO_SAMPLE* menuSample;
ALLEGRO_SAMPLE* jumpingSample;
ALLEGRO_SAMPLE* coinSample;
ALLEGRO_SAMPLE* moveSample;
ALLEGRO_SAMPLE* damegeSample;
ALLEGRO_SAMPLE* goldSample;

int main() {

	srand(time(NULL)),
		settings();
	font = al_load_font("font/font.ttf", 40, 0);
	font2 = al_load_font("font/arial.ttf", 20, 0);
	screen = al_create_display(w, h);
	imageRoad = al_load_bitmap("img/road.png");
	sun = al_load_bitmap("img/sun.png");
	coin = al_load_bitmap("img/gold.png");
	box = al_load_bitmap("img/block.png");


	al_reserve_samples(5);

	menuSample = al_load_sample("sound/selection.mp3");
	jumpingSample = al_load_sample("sound/jump.wav");
	moveSample = al_load_sample("sound/move.mp3");
	damegeSample = al_load_sample("sound/damage.mp3");
	goldSample = al_load_sample("sound/coin.mp3");
	while (play) {



		menu();
		playGame();



	}


	al_destroy_display(screen);
	al_destroy_font(font);
	al_destroy_font(font2);
	al_destroy_bitmap(imageRoad);
	al_destroy_bitmap(sun);
	al_destroy_bitmap(coin);
	al_destroy_sample(menuSample);
	al_destroy_sample(jumpingSample);
	al_destroy_sample(moveSample);
	al_destroy_sample(damegeSample);




}
//////////////////////////////////////END OF MAIN FUNCTION ///////////////////////////////////



//--------------------------------------------------------------------GamePLay function 


void playGame() {

	//FONT
	//BITMAPS
	///COLORS
	ALLEGRO_COLOR sky = al_map_rgb(25, 107, 202);
	ALLEGRO_COLOR grass = al_map_rgb(40, 151, 59);
	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
	ALLEGRO_COLOR orange = al_map_rgb(255, 165, 0);
	ALLEGRO_COLOR g = al_map_rgb(255, 215, 0);
	//RESIM BOYUTLARI
	int widthRoad = 620, heightRoad = 459;
	float ufuk = 140, ufukY = ufuk + 11;
	//EKRAN BOYUTLARI
	float WIDTH = widthRoad * 1.5;
	float HEIGHT = heightRoad * 1.5 + ufuk;
	//Yol Büyüme oraný(perspektif için )
	float scaleRate = 0.1;
	//QUEUE
	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
	//TIMER 
	ALLEGRO_TIMER* timerBlock = al_create_timer(0.6);
	ALLEGRO_TIMER* timerGold = al_create_timer(0.2);
	ALLEGRO_TIMER* timerMove = al_create_timer(0.035);
	ALLEGRO_TIMER* timerRoad = al_create_timer(0.4545);


	bool start = false;
	bool drawScreen = true;
	int fps = 60;
	bool jump = false;
	int missionLevel = 1;

	//TIMERS
	ALLEGRO_TIMER* timer;
	timer = al_create_timer(1.0 / fps);
	timerGold = al_create_timer(0.2);
	//QUEUE
	float roadScaledWidth = widthRoad * 1.5, roadScaledHeight = heightRoad * 1.5;
	float dx = 0;
	float dy = ufuk;
	int x = 0;
	int y = 0;
	//CAMERA SETTINGS
	float cameraXY[2] = { 0 , 0 };
	ALLEGRO_TRANSFORM camera;
	float tanGrass = tan(56.9 * 3.14159265 / 180); // sag ve sol yeþil alan için kullanýlacak sayý
	struct item player;
	player.x = WIDTH / 2 - 60;
	player.y = FLOOR;
	player.position = 1;




	int speedJump = JUMP_MAX_SPEED;

	struct item gold[TOTAL_GOLD];
	struct item block[10];
	initBlock(block, TOTAL_BLOCK);
	initBlock(gold, TOTAL_GOLD);

	if (game) {
		al_stop_timer(timer);
		al_stop_timer(timerBlock);
		al_stop_timer(timerMove);
		al_stop_timer(timerGold);
		al_stop_timer(timerRoad);
		al_flush_event_queue(queue);


	}





	else {
		al_register_event_source(queue, al_get_timer_event_source(timer));
		al_register_event_source(queue, al_get_timer_event_source(timerBlock));
		al_register_event_source(queue, al_get_timer_event_source(timerGold));
		al_register_event_source(queue, al_get_timer_event_source(timerMove));
		al_register_event_source(queue, al_get_timer_event_source(timerRoad));

		al_register_event_source(queue, al_get_keyboard_event_source());
		al_start_timer(timer);
		al_start_timer(timerBlock);
		al_start_timer(timerGold);

		al_start_timer(timerMove);






	}


	int j = 7;
	int t = 0;
	while (!game) {
	

		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);
		y++;
		// DONGU
		if (x == 22) {
			roadScaledWidth = widthRoad * 1.5;
			roadScaledHeight = heightRoad * 1.5;
			dx = 0;
			x = 0;
			dy = ufuk;

		}

		//KEYBOARD EVENTS

		if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			drawScreen = true;
			switch (event.keyboard.keycode) {

			case ALLEGRO_KEY_ESCAPE:
				game = 1;
				m = 1;

				break;
			case ALLEGRO_KEY_UP:
				KEY[UP] = true;

				break;

			case ALLEGRO_KEY_SPACE:
				KEY[UP] = true;

				break;

			case ALLEGRO_KEY_DOWN:
				KEY[DOWN] = true;
				break;
			case ALLEGRO_KEY_LEFT:
				KEY[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				KEY[RIGHT] = true;
				break;



			}

		}
		if (event.type == ALLEGRO_EVENT_KEY_UP) {
			drawScreen = true;
			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				KEY[UP] = false;
				break;
			case ALLEGRO_KEY_SPACE:
				KEY[UP] = false;
				break;
			case ALLEGRO_KEY_DOWN:
				KEY[DOWN] = false;
				break;
			case ALLEGRO_KEY_LEFT:
				KEY[LEFT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				KEY[RIGHT] = false;
				break;
			}

		}
		if (KEY[UP]) {
			if (!jump) {
				playSound(jumpingSample);
				jump = true;
			}
			//cameraXY[1] -= 35;

			KEY[UP] = false;

		}
		if (KEY[DOWN]) {

			//cameraXY[1] += 35;
			//if (cameraXY[1] >= 0)
			//	cameraXY[1] = 0;

			KEY[DOWN] = false;

		}
		if (KEY[LEFT] && player.position != 0) {

			//cameraXY[0] -= 35;

			KEY[LEFT] = false;
			player.x -= 230;
			player.position--;
			playSound(moveSample);
		}

		if (KEY[RIGHT] && player.position != 2) {

			//cameraXY[0] +=35;
			KEY[RIGHT] = false;
			player.x += 230;
			player.position++;
			playSound(moveSample);


		}

		player.x2 = player.x + 130;
		player.y2 = player.y + 150;
		
		al_identity_transform(&camera);
		al_translate_transform(&camera, -cameraXY[0], -cameraXY[1]);
		al_use_transform(&camera);
		if (event.timer.source == timer) {
			drawScreen = true;

		}
		if (event.timer.source == timerGold) {
			roadLoop(&dx, &dy, &roadScaledWidth, &roadScaledHeight);
			if (j == 14) {

				int iter = rand() % 4;
				while (iter == goldPos) {
					iter = rand() % 4;
				}
				goldPos = iter;

				j = rand() % 10;

			}
			
			createBlock(gold, 'g', TOTAL_GOLD);

			j++;




		}









		
			


		if (event.timer.source == timerBlock && rand() % 5 < 4) { //
			
			createBlock(block, 'b', TOTAL_BLOCK);
			if (rand() % 2 == 0)
				createBlock(block, 'b', TOTAL_BLOCK);
			if (rand() % 2 == 0)
				createBlock(block, 'b', TOTAL_BLOCK);
			createBlock(block, 'b', TOTAL_BLOCK);
			roadLoop(&dx, &dy, &roadScaledWidth, &roadScaledHeight);


		}







		if (event.timer.source == timerMove) { //
			moveBlocks(block);
			moveBlocks(gold);
			//moveGold(gold);
			roadLoop(&dx, &dy, &roadScaledWidth, &roadScaledHeight);
			if (collision(player, block)) {
				al_flush_event_queue(queue);
				al_clear_to_color(al_map_rgb(0, 0, 0));
				al_draw_text(font, al_map_rgb(255, 255, 255), 400, 400, 0, "GAME OVER");
				al_draw_textf(font2, al_map_rgb(255, 255, 255), 450, 450, 0, "SCORE %d", score);
				al_flip_display();
				al_rest(3);
				game = true;
				m = true;
			}
			collision(player, gold);



		}






		if (jump == true) {

			player.y -= speedJump;
			speedJump -= GRAVITY;
			if (speedJump < -JUMP_MAX_SPEED) {
				speedJump = JUMP_MAX_SPEED;
				jump = false;
			}
			if (player.y > FLOOR) {
				player.y = FLOOR;

			}
		}



		if (drawScreen && !game) {
			
			
			al_clear_to_color(al_map_rgb(0, 0, 0));
			roadLoop(&dx, &dy, &roadScaledWidth, &roadScaledHeight);
			al_draw_scaled_bitmap(imageRoad, 0, 0, widthRoad, heightRoad, dx, dy, roadScaledWidth, roadScaledHeight, 0);		//BITMAP - yol
															//yolu hareket ettir
			al_draw_filled_triangle(cameraXY[0], ufukY, (HEIGHT - ufukY) / 1.533, ufukY, cameraXY[0], HEIGHT - (cameraXY[0]) * tan(56.9 * 3.1415926 / 180), grass);								//sol yeþil alan
			al_draw_filled_triangle(WIDTH + cameraXY[0], ufukY, WIDTH + cameraXY[0], HEIGHT + cameraXY[0] * tan(56.9 * 3.1415926 / 180), WIDTH - (HEIGHT - ufukY) / 1.533, ufukY, grass);		//sað yeþil alan
			al_draw_filled_rectangle(cameraXY[0], cameraXY[1], WIDTH + cameraXY[0], ufukY, sky);

			//gökyüzü
			al_draw_bitmap(sun, -200, -100, 0);
			drawScreen = false;

			drawBlock(gold, TOTAL_GOLD);
			drawBlock(block, TOTAL_BLOCK);

			//drawBlock(gold, TOTAL_GOLD);

			al_draw_filled_rectangle(player.x, player.y, player.x2, player.y2, orange);//DRAW PLAYER
			drawPassedBlock(gold, TOTAL_GOLD);
			drawPassedBlock(block, TOTAL_BLOCK);


			al_draw_textf(font2, black,750, 20, 0, "SCORE %d", score);
			

		}










		al_flip_display();
		x++;
	}

	//LOOP END
	al_destroy_timer(timer);
	al_destroy_timer(timerBlock);
	al_destroy_timer(timerMove);
	al_destroy_event_queue(queue);
}
void settings() { //SETTING FUNCTION TO START ALLEGRO FUNCTION
	al_init();
	al_init_primitives_addon();
	al_install_keyboard();
	al_init_image_addon();
	al_init_native_dialog_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_audio();
	al_init_acodec_addon();

}
//------------------------------------------------------------------------------------------RoadLoop Function
void roadLoop(float* dx, float* dy, float* dw, float* dh) {

	*dx -= *dw * 0.005;
	*dy += *dw * 0.00023;

	*dw += *dw * 0.01;
	*dh += *dh * 0.01;

}
//----------------------------------------------------------------------------------------- initBlock  Function
void initBlock(struct item b[], int x) {
	struct item* ptr;

	for (int i = 0; i < x; i++) {
		ptr = &b[i];
		ptr->live = false;
		ptr->speed = 1;
		ptr->x = 0;
		ptr->y = 0;
		ptr->x2 = 0;
		ptr->y2 = 0;
	}
}
//-----------------------------------------------------------------------------------------moveBlock Function
void moveBlocks(struct item block[]) {
	struct item* ptr;
	for (int i = 0; i < TOTAL_BLOCK; i++) {
		ptr = &block[i];
		if (ptr->live == true) {
			ptr->y += ptr->speed;
			ptr->y2 += ptr->speed + ptr->edgeRate2;
			ptr->edgeRate2 += 0.15;
			if (ptr->position == 0) {
				ptr->x -= ptr->edgeRate / 2;
				ptr->x2 += ptr->edgeRate / 2;
				ptr->edgeRate += 0.15;
			}
			else if (ptr->position == 1) {
				ptr->x -= ptr->edgeRate;
				ptr->x2 -= ptr->edgeRate / 2;
				ptr->edgeRate += 0.29;
			}

			else if (ptr->position == 2) {
				ptr->x += ptr->edgeRate / 2;
				ptr->x2 += ptr->edgeRate;
				ptr->edgeRate += 0.29;
			}

			ptr->speed += 0.5;

			if (ptr->y < 250 && ptr->y < 500) {

				ptr->speed -= 0.01;
			}
			else if (ptr->y >= 250 && ptr->y < 600) {

				ptr->speed += 0.15;
			}
			else if (ptr->y >= 600 && ptr->y <= 900) {
				ptr->speed += 0.01;
			}
			if (ptr->y > h) {
				ptr->live = false;
				ptr->speed = 1;
				ptr->edgeRate = 1;
				ptr->edgeRate2 = 1;
			}
		}
	}
}
//------------------------------------------------------------------------------------------createBlock Function
void createBlock(struct item block[], char t, int total) {
	struct item* ptr;

	for (int i = 0; i < TOTAL_BLOCK; i++) {
		ptr = &block[i];
		if (ptr->live == false) {
			ptr->type = t;
			if (ptr->type == 'g')
				ptr->position = goldPos;
			else if (ptr->type == 'b')
				ptr->position = rand() % 3;
			int a = rand() % 2;
			ptr->edgeRate = 1;
			ptr->edgeRate2 = 1;
			switch (ptr->position) {
			case 0:// BLOCK at MIDDLE

				ptr->x = 465;
				ptr->y = 140;
				ptr->live = true;
				break;
			case 1:// BLOCK at LEFT
				ptr->x = 450;
				ptr->y = 140;
				ptr->live = true;
				break;
			case 2:// BLOCK at RIGHT
				ptr->x = 480;
				ptr->y = 140;
				ptr->live = true;
				break;
			case 3:
				ptr->live = false;


			}
			ptr->x2 = ptr->x + 5;
			ptr->y2 = ptr->y + 5;




			return;
		}
	}
}
//------------------------------------------------------------------------------------------drawBlock Function
void drawBlock(struct item block[], int total) {
	struct item* ptr;// Pointer of block structure

	for (int i = 0; i < total; i++) {
		ptr = &block[i];

		if (ptr->live == true && ptr->y <= FLOOR) {

			if (ptr->type == 'b') {
				al_draw_filled_rectangle(ptr->x, ptr->y, ptr->x2, ptr->y2, al_map_rgb(255, 0, 0));



			}
			else if (ptr->type == 'g') {
				
				al_draw_scaled_bitmap(coin, 0, 0, 100, 100, ptr->x, ptr->y, ptr->x2 - ptr->x, ptr->y2 - ptr->y, 0);
				
				//al_draw_filled_rectangle(ptr->x, ptr->y, ptr->x2, ptr->y2, al_map_rgb(255, 165, 0));
			}
		}
	}
}
//------------------------------------------------------------------------------------------drawPassedBlock Function
void drawPassedBlock(struct item block[], int total) {

	struct item* ptr;// Pointer of structure

	for (int i = 0; i < total; i++) {
		ptr = &block[i];  //Assigning address of the block to pointer

		if (ptr->live == true && ptr->y > FLOOR) {   // If the block exist and it is passed 

			if (ptr->type == 'b') {
				al_draw_filled_rectangle(ptr->x, ptr->y, ptr->x2, ptr->y2, al_map_rgb(255, 0, 0));
			}
			else if (ptr->type == 'g') {
				al_draw_scaled_bitmap(coin, 0, 0, 100, 100, ptr->x, ptr->y, ptr->x2 - ptr->x, ptr->y2 - ptr->y, 0);

			}

		}
	}
}
//------------------------------------------------------------------------------------------collision Function   to control that whether there is any collision
int collision(struct item player, struct item block[]) {
	struct  item* blockP;

	for (int i = 0; i < TOTAL_BLOCK; i++) {
		blockP = &block[i];
		if (blockP->type == 'b') {
			if (abs((int)(player.x - blockP->x)) < 50 && abs((int)(player.x2 - blockP->x2)) < 50 && abs((int)(player.y - blockP->y)) < 50 && (blockP->y > 530 && blockP->y < 650)) {
				playSound(damegeSample);
				return 1;
			}
		}



		else if (blockP->type == 'g' && blockP->live == true) {

			if ((abs((int)(player.x - blockP->x)) < 120 || abs((int)(player.x2 - blockP->x2)) < 120) && (abs((int)(player.y - blockP->y)) < 200) && (blockP->y > 500 && blockP->y < 650)) {

				blockP->live = false;
				blockP->speed = 1;
				blockP->edgeRate = 1;
				blockP->edgeRate2 = 1;
				score++;
				return 2;
			}
		}












	}
	return 0;
}









//------------------------------------------------------------------------------------------MENU FUNCTION            loads menu initially 
void menu() {

	if (score >= highscore) {
		highscore = score;
	}
	if (m) {


		ALLEGRO_EVENT_QUEUE* queueMenu = al_create_event_queue();
		ALLEGRO_COLOR m1 = al_map_rgb(255, 0, 0);
		ALLEGRO_COLOR m0 = al_map_rgb(255, 255, 255);


		int selected = 0;
		ALLEGRO_COLOR menuColors[] = { m1,m0,m0,m0,m0 };

		al_register_event_source(queueMenu, al_get_keyboard_event_source());
		al_flush_event_queue(queueMenu);


		bool help = false;
		bool about = false;


		while (m == true) {

			al_clear_to_color(al_map_rgb(0, 0, 0));

			ALLEGRO_EVENT eventMenu;
			al_wait_for_event_timed(queueMenu, &eventMenu, 0.2);

			bool key[] = { false , false };
			al_draw_line(0, 250, w, 250, al_map_rgb(255, 255, 255), 1);
			al_draw_textf(font, menuColors[0], 300, 300, 0, "PLAY");
			if (sound)
				al_draw_textf(font, menuColors[1], 300, 350, 0, "SOUND  ON");
			if (!sound)
				al_draw_textf(font, menuColors[1], 300, 350, 0, "SOUND  OFF");
			al_draw_textf(font, menuColors[2], 300, 400, 0, "HELP");
			al_draw_textf(font, menuColors[3], 300, 450, 0, "ABOUT");
			al_draw_textf(font, menuColors[4], 300, 500, 0, "EXIT");
			al_draw_line(0, 570, w, 570, al_map_rgb(255, 255, 255), 1);

			if (help)
			{	//al_draw_textf(font2, al_map_rgb(255, 255, 255), 100, 600, 0, "Use arrow keys to move . Gather coins and do not crash to red blocks ");
				al_draw_textf(font2, al_map_rgb(255, 255, 255), 200, 600, 0, "COLLECT GOLDS AND DO NOT CRASH TO RED BLOCKS ");
				
				al_draw_textf(font2, al_map_rgb(255, 255, 255), 300, 650, 0, "[RIGHT ARROW] MOVE RIGHT");
				al_draw_textf(font2, al_map_rgb(255, 255, 255), 300, 700, 0, "[LEFT ARROW]: MOVE LEFT");

				al_draw_textf(font2, al_map_rgb(255, 255, 255), 300, 750, 0, "[UP] / [SPACE] : JUMP");
				
				al_draw_textf(font2, al_map_rgb(255, 255, 255), 300, 800, 0, "[ESC]: MENU");

				
			}

			if (about) {
				al_draw_textf(font2, al_map_rgb(255, 255, 255), 200, 600, 0, "DEVELOPED by VELAT NECMETTIN KANAT");
				al_draw_textf(font2, al_map_rgb(255, 255, 255), 300, 650, 0, "2020");




			}
			al_draw_textf(font, al_map_rgb(255, 0, 255), 600, 10, 0, "Highest score %d", highscore);




			al_flip_display();
			if (eventMenu.type == ALLEGRO_EVENT_KEY_DOWN) {
				playSound(menuSample);
				switch (eventMenu.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					key[0] = true;
					break;
				case ALLEGRO_KEY_DOWN:
					key[1] = true;
					break;
				case ALLEGRO_KEY_ENTER:
					switch (selected) {
					case 0:

						m = false;
						game = false;

						break;
					case 1:
						sound = !sound;
						break;
					case 2:
						if (help)
							help = false;
						else if (!help) {
							help = true;
							about = false;
						}

						break;
					case 3:
						if (about)
							about = false;
						else if (!about) {
							about = true;
							help = false;
						}

						break;

					case 4:
						m = false;
						game = true;
						play = false;
						break;
					}
					break;

				}
			}
			if (key[0]) {
				menuColors[selected] = m0;
				selected = (selected - 1) % 5;
				if (selected < 0)
					selected = 4;
				if (selected > 4)
					selected = 0;
				menuColors[selected] = m1;
				

			}
			else if (key[1]) {
				menuColors[selected] = m0;
				selected = (selected + 1) % 5;
				if (selected < 0)
					selected = 4;
				if (selected > 4)
					selected = 0;
				menuColors[selected] = m1;
				
			}
		}
		al_destroy_event_queue(queueMenu);
	}
	score = 0;
}

void playSound(ALLEGRO_SAMPLE* sample) {
	if (sound == true) {

		al_play_sample(sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
	}
}





