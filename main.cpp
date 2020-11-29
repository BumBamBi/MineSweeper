#include<SFML/Graphics.hpp>
#include<iostream>
#include<ctime>

#define COL 16
#define ROW 16			// up to 11
#define BOMBS_CNT 10	// down to COL * ROW

#define BLANK 0
#define BOMB 9
#define UNCLICK 10
#define FLAG 11
#define WRONG 12
#define BOOM 13
#define WALL -99

#define TILE_PIXCEL 32
#define FACE_PIXCEL 106
#define TIMER_WIDTH_PIXCEL 80
#define TIMER_HEIGHT_PIXCEL 106

using namespace std;
using namespace sf;

class tile {
public:
	bool clicked;
	bool flag;
	bool clickMotion;
	int val;	// 0	1~8		9		10	11	12 13

	tile() {
		clicked = false;
		flag = false;
		clickMotion = false;
		val = -99;
	}

	int state() {
		if (clicked) {
			return val;
		}
		if (flag) {
			return FLAG;
		}
		if (clickMotion) {
			return BLANK;
		}
		else {
			return UNCLICK;
		}
	}
};

class face {
public:
	bool click;
	int val;

	face() {
		click = false;
		val = 0;
	}
};

tile table[COL + 2][ROW + 2];
face face_button;
bool coin = true;

// 초기화 -----------------------
void set_bombs() {
	int bombs_cnt = 0;
	while (bombs_cnt != BOMBS_CNT) {
		int x = rand() % ROW + 1;	// 1 ~ ROW
		int y = rand() % COL + 1;	// 1 ~ COL

		if (table[x][y].val != BOMB) {
			table[x][y].val = BOMB;
			bombs_cnt += 1;
		}
	}
}
void set_nums() {
	int cnt = 0;

	for (int i = 1; i <= COL; i++) {
		for (int j = 1; j <= ROW; j++) {
			if (table[i][j].val != BOMB) {

				// 팔방 확인
				for (int m = -1; m <= 1; m++) {
					for (int n = -1; n <= 1; n++) {
						if (m == 0 && n == 0) {
							continue;
						}
						if (table[i + m][j + n].val == BOMB) {
							cnt += 1;
						}
					}
				}
				table[i][j].val = cnt;
				cnt = 0;
			}
		}
	}
}
void init(Clock& _clock, int& _cur_time) {

	for (int i = 1; i <= COL; i++) {
		for (int j = 1; j <= ROW; j++) {
			table[i][j].clicked = false;
			table[i][j].clickMotion = false;
			table[i][j].flag = false;
			table[i][j].val = WALL;
		}
	}

	_clock.restart();
	_cur_time = 0;

	set_bombs();
	set_nums();
	
	coin = true;
	face_button.val = 0;

	for (int i = 0; i < 20; i++) {
		cout << endl;
	}
	// 타이머
	// 우승 관련 함수 등등
}
// ------------------------------


// 승패 -------------------------
int remainTiles_cnt() {
	int cnt = 0;

	for (int i = 1; i <= COL; i++) {
		for (int j = 1; j <= ROW; j++) {
			if (table[i][j].clicked == false) {
				cnt += 1;
			}
		}
	}
	return cnt;
}

bool isWin() {
	// 남은 UNCLICK개수(FLAG와 무관)가 폭탄개수와 동일하면 승리
	if (remainTiles_cnt() == BOMBS_CNT) {
		return true;
	}
	else {
		return false;
	}
}

void Win(int _cur_time) {
	if (coin) {
		coin = false;

		for (int i = 1; i <= COL; i++) {
			for (int j = 1; j <= ROW; j++) {
				table[i][j].flag = true;
			}
		}
		cout << "WIN" << endl;
		cout << "Your Record : " << _cur_time << "s" << endl;
	}
}

void Lose() {
	if (coin) {
		coin = false;

		for (int i = 1; i <= COL; i++) {
			for (int j = 1; j <= ROW; j++) {
				if (table[i][j].flag == true && table[i][j].val == BOMB) {
					table[i][j].val = FLAG;
				}
				else if (table[i][j].flag == true && table[i][j].val != BOMB) {
					table[i][j].val = WRONG;
				}
				table[i][j].clicked = true;
			}
		}
		face_button.val = 1;
		cout << "LOSE" << endl;
	}
}
// ------------------------------


// 기능 -------------------------
void open_blanks(int _i, int _j) {
	
	if (table[_i][_j].val == WALL) {
		return;
	}

	for (int m = -1; m <= 1; m++) {
		for (int n = -1; n <= 1; n++) {
			// 자기자신은 pass(이미 확인을 한 상태)
			if (m == 0 && n == 0) {
				continue;
			}
			else {
				// 값이 BLANK이고, flag가 아니라면
				if (table[_i + m][_j + n].val == BLANK && table[_i + m][_j + n].flag == false) {
					// 이미 열려있을 떄
					if (table[_i + m][_j + n].clicked == true) {
						continue;
					}
					// 그 외의 경우는, 칸을 열고, 다시 재귀 호출
					else {
						table[_i + m][_j + n].clicked = true;
						open_blanks(_i + m, _j + n);
					}
				}
				// 최 말단 숫자인 경우
				else if (table[_i + m][_j + n].val >= 1 && table[_i + m][_j + n].val <= 8) {
					table[_i + m][_j + n].clicked = true;
					continue;
				}
			}
		}
	}
}

void open_tile(int _i, int _j) {

	if (coin) {
		table[_i][_j].clicked = true;

		if (table[_i][_j].val == BLANK) {
			open_blanks(_i, _j);
		}
		else if (table[_i][_j].val == BOMB) {
			table[_i][_j].val = BOOM;
			Lose();
		}
	}
}

void open_8side_down(int _i, int _j) {
	for (int m = -1; m <= 1; m++) {
		for (int n = -1; n <= 1; n++) {
			if (m == 0 && n == 0) {
				continue;
			}
			else {
				if (table[_i + m][_j + n].flag == false) {
					table[_i + m][_j + n].clickMotion = true;
				}
			}
		}
	}
}

void open_8side_up(int _i, int _j) {
	int flags_cnt = 0;

	for (int m = -1; m <= 1; m++) {
		for (int n = -1; n <= 1; n++) {
			if (m == 0 && n == 0) {
				continue;
			}
			else {
				if (table[_i + m][_j + n].flag == false) {
					table[_i + m][_j + n].clickMotion = false;
				}
				else {
					flags_cnt += 1;
				}
			}
		}
	}

	if (table[_i][_j].val == flags_cnt) {
		for (int m = -1; m <= 1; m++) {
			for (int n = -1; n <= 1; n++) {
				if (m == 0 && n == 0) {
					continue;
				}
				else {
					if (table[_i + m][_j + n].flag == false) {
						open_tile(_i + m, _j + n);
					}
				}
			}
		}
	}
}
// ------------------------------

void claTimer(RenderWindow& _window, Sprite& _img_num, int _cur_time) {

	if (_cur_time <= 999) {
		int timer_1 = _cur_time % 10;
		int timer_10 = (_cur_time / 10) % 10;
		int timer_100 = (_cur_time / 100) % 10;

		int time_dec[3] = { timer_100, timer_10, timer_1 };

		for (int i = 0; i < 3; i++) {
			_img_num.setTextureRect(IntRect(TIMER_WIDTH_PIXCEL * time_dec[i], 0, TIMER_WIDTH_PIXCEL, TIMER_HEIGHT_PIXCEL));
			_img_num.setPosition(FACE_PIXCEL + (TIMER_WIDTH_PIXCEL * i), 0);
			_window.draw(_img_num);
		}
	}
}

void show() {
	for (int i = 1; i <= COL; i++) {
		for (int j = 1; j <= ROW; j++) {
			if (table[i][j].val == BOMB) {
				cout << "* ";
			}
			else {
				cout << table[i][j].val << " ";
			}
		}
		cout << endl;
	}
	cout << "===============================" << endl;
}

int main() {

	if (ROW < 11) {
		cout << "plz change ROW Value (up to 11)" << endl;
		return 0;
	}
	if (BOMBS_CNT >= COL * ROW) {
		cout << "plz reduce BOMBS_CNT Value (down to COL*ROW)" << endl;
		return 0;
	}

	srand((unsigned)time(0));

	// 타이머
	Clock clock;
	Time timer;
	int cur_time = 0;

	// 초기 세팅
	init(clock, cur_time);
	show();

	RenderWindow window(VideoMode(TILE_PIXCEL * ROW, TILE_PIXCEL * COL + FACE_PIXCEL), "lkw");

	// 이미지 Texture/Sprite 생성 -----
	Texture img_tiles;
	img_tiles.loadFromFile("images/tiles.jpg");
	Sprite img_tile(img_tiles);

	Texture img_faces;
	img_faces.loadFromFile("images/faces.jpg");
	Sprite img_face(img_faces);

	Texture img_nums;
	img_nums.loadFromFile("images/nums.jpg");
	Sprite img_num(img_nums);
	// --------------------------------

	// 변수 ---------------------------
	Vector2i location_xy;
	int arr_i, arr_j;
	// --------------------------------

	while (window.isOpen()) {

		timer = clock.getElapsedTime();

		Event e;

		if (window.pollEvent(e)) {
			if (e.type == Event::Closed) {
				window.close();
			}
			if (e.type == Event::MouseButtonPressed) {
				if (e.key.code == Mouse::Left) {
					location_xy = Mouse::getPosition(window);
					
					// 얼굴 관련 클릭 코드
					if (location_xy.x <= FACE_PIXCEL && location_xy.y <= FACE_PIXCEL) {
						face_button.click = true;
						if (face_button.val == 0) {
							face_button.val = 2;
						}
						else if (face_button.val == 1) {
							face_button.val = 3;
						}
					}

					// 타일 관련 클릭 코드
					if (location_xy.y > FACE_PIXCEL) {
						arr_i = (location_xy.y - FACE_PIXCEL) / TILE_PIXCEL + 1;
						arr_j = location_xy.x / TILE_PIXCEL + 1;

						if (table[arr_i][arr_j].clicked) {
							// 팔방 확인 함수
							open_8side_down(arr_i, arr_j);
						}
						else {
							// 타일을 열어주는 부분
							open_tile(arr_i, arr_j);
						}
					}

					if (isWin()) {
						Win(cur_time);
					}
				}
				if (e.key.code == Mouse::Right) {
					location_xy = Mouse::getPosition(window);

					arr_i = (location_xy.y - FACE_PIXCEL) / TILE_PIXCEL + 1;
					arr_j = location_xy.x / TILE_PIXCEL + 1;

					if (table[arr_i][arr_j].clicked == false) {
						table[arr_i][arr_j].flag = !table[arr_i][arr_j].flag;
					}
				}
			}
			if (e.type == Event::MouseButtonReleased) {
				if (e.key.code == Mouse::Left) {
					// 얼굴
					if (location_xy.x <= FACE_PIXCEL && location_xy.y <= FACE_PIXCEL) {
						face_button.click = false;
						face_button.val = 0;
						init(clock, cur_time);
						show();
					}

					// 타일
					if (location_xy.y > FACE_PIXCEL) {
						open_8side_up(arr_i, arr_j);
					}

					if (isWin()) {
						Win(cur_time);
					}
				}
			}

			// 얼굴 버튼 그리기
			img_face.setTextureRect(IntRect(FACE_PIXCEL * face_button.val, 0, FACE_PIXCEL, FACE_PIXCEL));
			img_face.setPosition(0, 0);
			window.draw(img_face);

			// 타이머
			claTimer(window, img_num, cur_time);

			// 타일 그리기
			for (int i = 1; i <= COL; i++) {
				for (int j = 1; j <= ROW; j++) {
					img_tile.setTextureRect(IntRect(TILE_PIXCEL * table[i][j].state(), 0, TILE_PIXCEL, TILE_PIXCEL));
					img_tile.setPosition(TILE_PIXCEL * (j-1), TILE_PIXCEL * (i-1) + FACE_PIXCEL);
					window.draw(img_tile);
				}
			}

			// 보여주기
			window.display();
		}
		else {
			if (coin) {
				if (cur_time < (int)timer.asSeconds()) {

					// 타이머
					cur_time = (int)timer.asSeconds();
					claTimer(window, img_num, cur_time);

					// 얼굴
					img_face.setTextureRect(IntRect(FACE_PIXCEL * face_button.val, 0, FACE_PIXCEL, FACE_PIXCEL));
					img_face.setPosition(0, 0);
					window.draw(img_face);

					// 타일
					for (int i = 1; i <= COL; i++) {
						for (int j = 1; j <= ROW; j++) {
							img_tile.setTextureRect(IntRect(TILE_PIXCEL * table[i][j].state(), 0, TILE_PIXCEL, TILE_PIXCEL));
							img_tile.setPosition(TILE_PIXCEL * (j - 1), TILE_PIXCEL * (i - 1) + FACE_PIXCEL);
							window.draw(img_tile);
						}
					}

					window.display();
				}
			}
		}
	}

	return 0;
}