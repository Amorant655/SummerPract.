#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include "rapidjson\document.h"
#define CONST_COORD_VAL_1 55.73203
#define CONST_COORD_VAL_2 -0.0000048974
#define CONST_COORD_VAL_3 37.66491
#define CONST_COORD_VAL_4 0.000035725

using namespace cv;
using namespace std;
using namespace rapidjson;

class Coords {
public:
	Mat filename;
	vector<Point> coords;
	vector<double> map_coords_x;
	vector<double> map_coords_y;
	int N = 2;
	Mat copy_img;
	ofstream save_filename;
public:
	Coords(Mat nname) {
		filename = nname;
		save_filename.open("Coordinates.txt", ios::app);
	}
	void mouse_call(int  event, int  x, int  y, int  flag){
		if (event == EVENT_LBUTTONDOWN){
			coords.push_back(Point(x, y));
			map_coords_x.push_back(CONST_COORD_VAL_1 + CONST_COORD_VAL_2 * x);
			map_coords_y.push_back(CONST_COORD_VAL_3 + CONST_COORD_VAL_4 * y);

			if (coords.size() == 2) {
				copy(coords.begin(), coords.end(), ostream_iterator<Point>(cout, " "));
				cout << "  <- Pixels" << endl;
				save_filename << coords[0] << " " << coords[1] << " <-Pixels" << endl;
				cout << "[" << map_coords_x[0] << ", " << map_coords_y[0] << "] ";
				cout << "[" << map_coords_x[1] << ", " << map_coords_y[1] << "] ";
				cout << "  <- Coordinates" << endl;
				save_filename << map_coords_x[0] << " " << map_coords_y[0] << " ";
				save_filename << map_coords_x[1] << " " << map_coords_y[1] << " <-Coordinates" << endl;
			}


		}
	}
	void drawPoint(Mat img, Point p){
		circle(img, p, 5, Scalar(0, 0, 0), -1);
	}
	void drawLine(Mat img, Point p1, Point p2) {
		line(img, p1, p2, Scalar(0, 0, 0), 2);
		imshow("Copy2", img);
	}
	void drawRectangle(Mat img, Point p1, Point p2) {
		rectangle(img, p1, p2, Scalar(0, 0, 255), 2);
		img.copyTo(copy_img);
		imshow("Copy", img);
	}
};

Coords* rec_ptr;

void onMouseRec(int event, int x, int y, int flag, void* param) {
	rec_ptr->mouse_call(event, x, y, flag);
	if (rec_ptr->coords.size() == rec_ptr->N){
		rec_ptr->drawRectangle(rec_ptr->filename, rec_ptr->coords[0], rec_ptr->coords[1]);
		destroyWindow("Image");
		int key = waitKey(0) & 255;
		if (key == 13) { 
			destroyWindow("Copy");
		}; 
		rec_ptr->coords.clear();
		bool* exitflag = static_cast<bool*>(param);
		*exitflag = true;
	}
}

Coords* line_ptr;

void onMouseLine(int event, int x, int y, int flag, void* param) {
	line_ptr->mouse_call(event, x, y, flag);
	if (line_ptr->coords.size() == line_ptr->N) {
		line_ptr->drawPoint(line_ptr->filename, line_ptr->coords[0]);
		line_ptr->drawPoint(line_ptr->filename, line_ptr->coords[1]);
		line_ptr->drawLine(line_ptr->filename, line_ptr->coords[0], line_ptr->coords[1]);
		int key = waitKey(0) & 255;
		if (key == 13) {
			destroyWindow("Copy2");
		}
		if (key == 27) {
			destroyAllWindows();
		}
		if (line_ptr->coords.size() == line_ptr->N) {
			Point fs = line_ptr->coords[1];
			double _x = line_ptr->map_coords_x[1];
			double _y = line_ptr->map_coords_y[1];
			line_ptr->coords.clear();
			line_ptr->coords.push_back(fs);
			line_ptr->map_coords_x.clear();
			line_ptr->map_coords_x.push_back(_x);
			line_ptr->map_coords_y.clear();
			line_ptr->map_coords_y.push_back(_y);
		}
		bool* exitflag = static_cast<bool*>(param);
		*exitflag = true;
	}
}

vector<Point> drawRec() {
	double coord_x;  // координаты ширины
	double coord_y;  // координаты высоты
	int pixel_x; // координаты в пикселях
	int pixel_y;
	vector<Point> coords; // массив координат
	cout << "Input first rectangle coordinates: " << endl;
	cin >> coord_x;
	cin >> coord_y;
	pixel_x = (coord_x - CONST_COORD_VAL_1) / (CONST_COORD_VAL_2);
	pixel_y = (coord_y - CONST_COORD_VAL_3) / (CONST_COORD_VAL_4);
	coords.push_back(Point(pixel_x, pixel_y));
	cout << endl;
	cout << "Input second rectangle coordinates: " << endl;
	cin >> coord_x;
	cin >> coord_y;
	pixel_x = (coord_x - CONST_COORD_VAL_1) / (CONST_COORD_VAL_2);
	pixel_y = (coord_y - CONST_COORD_VAL_3) / (CONST_COORD_VAL_4);
	coords.push_back(Point(pixel_x, pixel_y));
	cout << endl;
	return coords;
}

void fileRead(string filename_map, string filename_points, bool rec,int Key , Point p1 = Point(0, 0), Point p2 = Point(0, 0)) { 
	string filename_m = filename_map;
	string filename_p = filename_points;
	Mat img = imread(filename_m);
	resize(img, img, Size(), 1.2, 1.2);

	fstream file_input(filename_p, ios_base::in);
	int seconds;

	if (rec) {
		vector<Point> coords;
		coords = drawRec();
		rectangle(img, coords[0], coords[1], Scalar(0, 0, 255), 2);
	}
	else {
		rectangle(img, p1, p2, Scalar(0, 0, 255), 2);
	}

	float buf;
	int _x;
	int _y;
	vector<Point> coordinates;
	bool ind = false;
	while (file_input >> buf) {
		if (ind) {
			ind = false;
			_y = (buf - CONST_COORD_VAL_3) / (CONST_COORD_VAL_4);
			coordinates.push_back(Point(_x, _y));
		}
		else {                                                                                                                 
			ind = true;
			_x = (buf - CONST_COORD_VAL_1) / (CONST_COORD_VAL_2);
		}
	}
	if (Key == 1) {
		cout << "How many seconds would you like to wait: ";
		cin >> seconds;
	}
	namedWindow("Image");
	for (int i = 0; i < coordinates.size(); i++) {
		circle(img, coordinates[i], 5, Scalar(0, 0, 0), -1); 
		if (Key == 1) {
			imshow("Image", img);
			waitKey(1000*seconds);
		}
	}
	imshow("Image", img);
	waitKey(0);
}

void consoleWrite() {
	ofstream save_filename("Coordinates.txt");

	string filename;
	cout << "Filename for map: ";
	cin >> filename;
	Mat img = imread(filename);
	resize(img, img, Size(), 1.2, 1.2);

	double coord_x;
	double coord_y;
	int pixel_x;
	int pixel_y;
	vector<Point> coords;
	int N;
	cout << "Input first rectangle coordinates: " << endl;
	cin >> coord_x;
	cin >> coord_y;
	pixel_x = (coord_x - CONST_COORD_VAL_1) / (CONST_COORD_VAL_2);
	pixel_y = (coord_y - CONST_COORD_VAL_3) / (CONST_COORD_VAL_4);
	save_filename << pixel_x << " " << pixel_y << " <-Pixels" << endl;
	save_filename << coord_x << " " << coord_y << " <-Coordinates" << endl;
	coords.push_back(Point(pixel_x, pixel_y));
	cout << endl;
	cout << "Input second rectangle coordinates: " << endl;
	cin >> coord_x;
	cin >> coord_y;
	pixel_x = (coord_x - CONST_COORD_VAL_1) / (CONST_COORD_VAL_2);
	pixel_y = (coord_y - CONST_COORD_VAL_3) / (CONST_COORD_VAL_4);
	coords.push_back(Point(pixel_x, pixel_y));
	cout << endl;
	save_filename << pixel_x << " " << pixel_y << " <-Pixels" << endl;
	save_filename << coord_x << " " << coord_y << " <-Coordinates" << endl;
	rectangle(img, coords[0], coords[1], Scalar(0, 0, 255), 2);
	cout << "Enter number of points: ";
	cin >> N;
	for (int i = 2; i < N + 2; i++) {
		cout << "Input point coordinates: " << endl;
		cin >> coord_x;
		cin >> coord_y;
		pixel_x = (coord_x - CONST_COORD_VAL_1) / (CONST_COORD_VAL_2);
		pixel_y = (coord_y - CONST_COORD_VAL_3) / (CONST_COORD_VAL_4);
		coords.push_back(Point(pixel_x, pixel_y));
		cout << endl;
		save_filename << pixel_x << " " << pixel_y << " <-Pixels" << endl;
		save_filename << coord_x << " " << coord_y << " <-Coordinates" << endl;
		circle(img, coords[i], 5, Scalar(0, 0, 0), -1);
	}
	namedWindow("Image");
	imshow("Image", img);
	waitKey(0);
	cout << "Your coordinates is saved in Coordinates.txt " << endl;
	save_filename.close();
}

void jsonRead() { 
	ifstream json("json_coords.json");

	vector<double> _x;
	vector<double> _y;
	vector<int> _x_p;
	vector<int> _y_p;
	vector<Point> coords;
	bool ind = false;
	bool pixel = false;

	string line, text;
	while (getline(json, line)) {
		text += line;
	}
	const char* data = text.c_str();
	Document document;
	document.Parse(data);
	assert(document.IsObject());
	assert(document.HasMember("Image"));
	assert(document["Image"].IsString());
	string filename_map = document["Image"].GetString();
	Mat img = imread(filename_map);
	assert(document.HasMember("FilePoints"));
	assert(document["FilePoints"].IsString());
	string filename_points = document["FilePoints"].GetString();
	assert(document.HasMember("Key"));
	assert(document["Key"].IsInt());
	int Key = document["Key"].GetInt();
	assert(document.HasMember("PointRectangle1"));
	const Value& PointRectangle1 = document["PointRectangle1"];
	assert(PointRectangle1.IsArray());
	for (SizeType i = 0; i < PointRectangle1.Size(); i++) {
		if (ind) {
			ind = false;
			if (!pixel) {
				_y.push_back(PointRectangle1[i].GetDouble());
				pixel = true;
			}
			else {
				_y_p.push_back(PointRectangle1[i].GetDouble());
				pixel = false;
			}
		}
		else {
			ind = true;
			if (!pixel) {
				_x.push_back(PointRectangle1[i].GetDouble());
			}
			else {
				_x_p.push_back(PointRectangle1[i].GetDouble());
			}
		}
	}
	const Value& PointRectangle2 = document["PointRectangle2"];
	assert(PointRectangle2.IsArray());
	for (SizeType i = 0; i < PointRectangle2.Size(); i++) {
		if (ind) {
			ind = false;
			if (!pixel) {
				_y.push_back(PointRectangle2[i].GetDouble());
				pixel = true;
			}
			else {
				_y_p.push_back(PointRectangle2[i].GetDouble());
			}
		}
		else {
			ind = true;
			if (!pixel) {
				_x.push_back(PointRectangle2[i].GetDouble());
			}
			else {
				_x_p.push_back(PointRectangle2[i].GetDouble());
			}
		}
	}
	for (int i = 0; i < 2; i++) {
		coords.push_back(Point(_x_p[i], _y_p[i]));
	}
	fileRead(filename_map, filename_points, false, Key, coords[0], coords[1]);
}

int main()
{
	int check;
	cout << "If you want to set coordinates using(1 by default): console write 1, mouse write 2, file read 3, json file read 4: ";
	cin >> check;
	if (check == 4) {
		jsonRead();
	}
	else if (check == 2) {
			bool rExit = false;

			string filename;
			cout << "Filename for map: ";
			cin >> filename;
			Mat img = imread(filename);

			ofstream save_file;
			save_file.open("Coordinates.txt");
			save_file.close();

			if (img.empty()) {
				cout << "Could not open or find file" << endl;
				cin.get();
				return -1;
			}

			resize(img, img, Size(), 1.2, 1.2);

			namedWindow("Image");
			Coords coords(img);
			rec_ptr = &coords;


			setMouseCallback("Image", onMouseRec, static_cast<void*>(&rExit));

			imshow("Image", img);

			while (!rExit) {
				waitKey(30);
			}

			bool lExit = false;

			namedWindow("Copy1");
			Coords copy_coords(coords.copy_img);
			line_ptr = &copy_coords;


			setMouseCallback("Copy1", onMouseLine, static_cast<void*>(&lExit));

			imshow("Copy1", coords.filename);

			while (!lExit) {
				waitKey(30);
			}

			cout << "Your coordinates is saved in Coordinates.txt " << endl;
			coords.save_filename.close();
			copy_coords.save_filename.close();
			cin.get();

		}
		else if (check == 3) {
			string filename_map, filename_points;
			int Key = 0;
			cout << "Filename for map: ";
			cin >> filename_map;
			cout << "Filename for points: ";
			cin >> filename_points;
			cout << "If you want display points immediatly enter 0, otherwise 1: ";
			cin >> Key;
			thread file_thr(fileRead, filename_map, filename_points, true, Key, Point(0, 0), Point(0, 0));
			if (file_thr.joinable())
				file_thr.join();
		}
		else {
			thread json_thr(consoleWrite);
			if (json_thr.joinable())
				json_thr.join();
			cin.get();

		}
	return(0);
}


