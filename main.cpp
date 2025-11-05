// main.cpp

#include <iostream>
#include <C:/OpenGLwrappers/glew-1.10.0-win32/glew-1.10.0/include/GL/glew.h>
#include <C:/OpenGLwrappers/freeglut-MSVC-2.8.1-1.mp/freeglut/include/GL/freeglut.h> 
#include "OpenGL445Setup-2025.h"
#include <math.h>
#include <string>


// Customize below 3 items to make canvas of own size & labeling
#define canvas_Width 1280
#define canvas_Height 720
char canvas_Name[] = "Solar Sytem";
const float CENTER_X = canvas_Width / 2.0f;
const float CENTER_Y = canvas_Height / 2.0f;

const float DEFAULT_POS[3] = {0.0f, 0.0f, 5000.0f};
float camera_pos[3]     = { DEFAULT_POS[0], DEFAULT_POS[1], DEFAULT_POS[2] };
float d_camera_pos[3]   = { 0.0f,           0.0f,           0.0f };
float camera_target[3]  = { DEFAULT_POS[0], DEFAULT_POS[1], 0.0f };
float movement_speed = 100.0f;

int num_objects = 1;
int current_object = 0;

// Time variables:
const float YEAR  = 5.0f; // Number of seconds desired to represent a year.
const float DAY   = (YEAR * 1000.0f) / 365.0f; // Number of milliseconds for a day.
const float DT    = 1000.0f / 60.0f; // Milliseconds to achieve 60fps

std::string sphere_mode = "WIRE"; // WIRE or SOLID

const GLint NUM_SLICES = 20;
const GLint NUM_STACKS = 20;

float x_orientation = 20.0f; // Previous 110.0f when x and y


//float star_radius = 30.0f;
float earth_radius = 10.0f;

const float PI = 3.14159265358979323846264338;
const float TWO_PI = 2.0f * PI;

static float hours_to_days(float hours)
{
  return (hours / 24.0f);
}

static std::string fill_str(std::string str, int width, char fill)
{
  std::string result = str;
  int i = result.length();
  if (i > width)
  {
    result = result.erase(width, result.length() - 1);
  }
  while (i < width)
  {
    result += fill;
    i++;
  }
  return result;
}

static float radians_to_degrees(float radians)
{
  return (radians * (180.0f / PI));
}

static float size_relative_to_earth(float diameter_in_km)
{
  return ((diameter_in_km/2.0f) / 12756.0f) * earth_radius;
}

float star_radius     = size_relative_to_earth(1391400);
float mercury_radius  = size_relative_to_earth(4879);
float venus_radius    = size_relative_to_earth(12104);
float mars_radius     = size_relative_to_earth(6792);
float jupiter_radius  = size_relative_to_earth(142984);
float saturn_radius   = size_relative_to_earth(120536);
float uranus_radius   = size_relative_to_earth(51118);
float neptune_radius  = size_relative_to_earth(49528);

float star_pos[3] = { 0.0f, 0.0f, 0.0f };
//const float AU = star_radius * (149600000.0f / (1391400.0 / 2.0f)); // Arbitrary number for astronomical unit.
const float AU = 70.0f;


/*
Planet Diameter (km) Distance from Sun (km)
Sun       1,391,400     -
Mercury   4,879     57,900,000
Venus     12,104    108,200,000
Earth     12,756    149,600,000
Mars      6,792     227,900,000
Jupiter   142,984   778,600,000
Saturn    120,536   1,433,500,000
Uranus    51,118    2,872,500,000
Neptune   49,528    4,495,100,000
*/

float star_color[3] = {1.0f, 1.0f, 0.0f};

float current_day = 0.0f;

int num_planets = 0;

static void draw_centered_bitmap(std::string message, float x, float y, float z)
{
  /*
  // Save current matrices
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, canvas_Width, 0, canvas_Height, 0.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);
  //*/

  // Draw bitmap
  glRasterPos3f(float(x) - (8.0f * message.length() / 2.0f), float(y), -1.0f);
  for (char c : message)
  {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
  }

  /*
  // Restore previous matrices
  glEnable(GL_DEPTH_TEST);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  gluPerspective(60.0, (GLdouble)canvas_Width / (GLdouble)canvas_Height, 1.0, 50000.0);
  glMatrixMode(GL_MODELVIEW);
  //*/
}

//static void draw_stroke_string(std::string message)

struct Planet
{
  float radius        = 1.0f; // Pixels
  float angle         = 0.0f; // Radians
  float days_to_orbit = 365.0f;
  float distance      = 0.0f; // au (Astronomical Units)
  float color[3]      = {1.0f, 1.0f, 1.0f};
  float pos[3]        = {0.0f, 0.0f, 0.0f};
  int num_orbits      = 0;
  int num_days        = 0;
  int num_moons       = 0;
  float rotation_dir  = 1.0f; // -1.0f if planet rotates clockwise (Venus)
  float orientation   = 0.0f; // Rotation radians
  float day_length    = 1.0f; // Days to complete a full rotation.
  std::string name    = "Unnamed Planet";

  float* moons_radius       = NULL; // Pixels
  float* moons_distance     = NULL; // Pixels
  float* moons_angle        = NULL; // Radians
  float* moons_orbit_days   = NULL;
  std::string* moons_names  = NULL;

  bool draw_orbit = true;
  bool draw_name = false;

  void calculate_pos()
  {
    /*
    pos[0] = star_pos[0] + (((distance * AU) + star_radius) * cosf(angle));
    pos[1] = star_pos[1] + (((distance * AU) + star_radius) * sinf(angle));
    pos[2] = star_pos[2];
    */

    pos[0] = star_pos[0] + (((distance * AU) + star_radius) * cosf(angle));
    pos[1] = star_pos[1];
    pos[2] = star_pos[2] + (((distance * AU) + star_radius) * sinf(angle));
  }

  Planet(float in_radius, float distance_from_star, int in_num_moons, float hours_to_rotate)
  {
    radius    = in_radius;
    distance  = distance_from_star;
    num_moons = in_num_moons;
    day_length = hours_to_days(hours_to_rotate);

    if (num_moons > 0)
    {
      moons_radius      = new float[num_moons];
      moons_distance    = new float[num_moons];
      moons_angle       = new float[num_moons];
      moons_orbit_days  = new float[num_moons];
      moons_names       = new std::string[num_moons];
      for (int i = 0; i < num_moons; i++)
      {
        moons_radius[i]     = 0.0f;
        moons_distance[i]   = 0.0f;
        moons_angle[i]      = 0.0f;
        moons_orbit_days[i] = 365.0f;
        moons_names[i]      = "Unnamed Moon";
      }
    }

    calculate_pos();
  }

  Planet()
  {
    // Do nothing. Default values already set.
  }

  Planet& operator=(const Planet& planet)
  {
    radius      = planet.radius;
    distance    = planet.distance;
    num_moons   = planet.num_moons;
    day_length  = planet.day_length;

    moons_radius      = new float[num_moons];
    moons_distance    = new float[num_moons];
    moons_angle       = new float[num_moons];
    moons_orbit_days  = new float[num_moons];
    moons_names       = new std::string[num_moons];

    pos[0] = planet.pos[0];
    pos[1] = planet.pos[1];
    pos[2] = planet.pos[2];

    color[0] = planet.color[0];
    color[1] = planet.color[1];
    color[2] = planet.color[2];

    for (int i = 0; i < planet.num_moons; i++)
    {
      moons_radius[i]     = planet.moons_radius[i];
      moons_distance[i]   = planet.moons_distance[i];
      moons_angle[i]      = planet.moons_angle[i]; 
      moons_orbit_days[i] = planet.moons_orbit_days[i];
      moons_names[i]      = planet.moons_names[i];
    }

    return *this;
  }

  ~Planet()
  {
    delete[] moons_radius;
    delete[] moons_distance;
    delete[] moons_angle;
  }

  void set_color(float r, float g, float b)
  {
    color[0] = r;
    color[1] = g;
    color[2] = b;
  }

  void set_color_hex(std::string hex)
  {
    for (int i = 0; i < 6; i += 2)
    {
      char c1 = hex[i]; char c2 = hex[i + 1];
      int v1  = 0;      int v2 = 0;
      switch (tolower(c1))
      {
      case 'a':
        v1 = 10; break;
      case 'b':
        v1 = 11; break;
      case 'c':
        v1 = 12; break;
      case 'd':
        v1 = 13; break;
      case 'e':
        v1 = 14; break;
      case 'f':
        v1 = 15; break;
      default:
        if (isdigit(c1)) { v1 = c1 - '0'; }
        break;
      }
      switch (tolower(c2))
      {
      case 'a':
        v2 = 10; break;
      case 'b':
        v2 = 11; break;
      case 'c':
        v2 = 12; break;
      case 'd':
        v2 = 13; break;
      case 'e':
        v2 = 14; break;
      case 'f':
        v2 = 15; break;
      default:
        if (isdigit(c2)) { v2 = c2 - '0'; }
        break;
      }
      int value = (v1 * 16) + v2;
      color[i/2] = float(value) / 255.0f;
    }
  }

  void draw()
  {
    /*
    float x = star_pos[0] + ( ((distance * AU) + star_radius) * cosf(angle) );  pos[0] = x;
    float y = star_pos[1] + ( ((distance * AU) + star_radius) * sinf(angle) );  pos[1] = y;
    float z = star_pos[2];                                                      pos[2] = z;
    */

    // Draw planet
    glColor3fv(color);
    glPushMatrix();
    //glRotatef(x_orientation, 1.0f, 0.0f, 0.0f);
    glTranslatef(pos[0], pos[1], pos[2]);
    //glRotatef(x_orientation, 1.0f, 0.0f, 0.0f);
    glRotatef(radians_to_degrees(orientation), 0.0f, 1.0f, 0.0f);
    if      (sphere_mode == "WIRE")   { glutWireSphere(radius, NUM_SLICES, NUM_STACKS); }
    else if (sphere_mode == "SOLID")  { glutSolidSphere(radius, NUM_SLICES, NUM_STACKS); }
    glPopMatrix();

    if (draw_name)
    {
      glColor3f(1.0f, 1.0f, 1.0f);
      glPushMatrix();
      //draw_centered_bitmap(name, pos[0] + radius, pos[1] + radius, pos[2] + radius);
      glTranslatef(pos[0] + radius, pos[1] + radius, pos[2] + radius);
      if (current_object == 0) { glRotatef(-x_orientation, 1.0f, 0.0f, 0.0f); }
      glScalef(0.3f, 0.3f, 0.3f);
      for (char c : name) { glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, c); }
      glPopMatrix();
    }

    if (draw_orbit)
    {
      // Draw ring of orbit
      glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
      int num_points = 100;
      glBegin(GL_LINE_LOOP);
      float d_angle = TWO_PI / num_points;
      float c_angle = 0.0f;
      for (int i = 0; i < num_points; i++)
      {
        /*
        glVertex3f(
          star_pos[0] + (((distance * AU) + star_radius) * cosf(c_angle)),
          star_pos[1] + (((distance * AU) + star_radius) * sinf(c_angle)),
          star_pos[2]
        );*/
        glVertex3f(
          star_pos[0] + (((distance * AU) + star_radius) * cosf(c_angle)),
          star_pos[1],
          star_pos[2] + (((distance * AU) + star_radius) * sinf(c_angle))
        );

        c_angle += d_angle;
      }
      glEnd();
    }

    // Draw moons
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < num_moons; i++)
    {
      /*
      float moon_x = pos[0] + (moons_distance[i] * cosf(moons_angle[i]));
      float moon_y = pos[1] + (moons_distance[i] * sinf(moons_angle[i]));
      float moon_z = pos[2];
      */
      float moon_x = pos[0] + (moons_distance[i] * cosf(moons_angle[i]));
      float moon_y = pos[1];
      float moon_z = pos[2] + (moons_distance[i] * sinf(moons_angle[i]));

      glPushMatrix();
      //glRotatef(x_orientation, 1.0f, 0.0f, 0.0f);
      glTranslatef(moon_x, moon_y, moon_z);
      //glRotatef(x_orientation, 1.0f, 0.0f, 0.0f);
      if      (sphere_mode == "WIRE")   { glutWireSphere(moons_radius[i], NUM_SLICES, NUM_STACKS); }
      else if (sphere_mode == "SOLID")  { glutSolidSphere(moons_radius[i], NUM_SLICES, NUM_STACKS); }
      glPopMatrix();
    }
  }
  
  void do_orbit()
  {
    if (days_to_orbit == 0.0f) { return; }
    float rot_speed = TWO_PI / (days_to_orbit * (DAY/DT));
    /*
      TWO_PI / days_to_orbit = rotation radians to complete rotation in 1 year

    */
    angle += rot_speed;
    while (angle >= TWO_PI)
    {
      angle -= TWO_PI;
      num_orbits++;
    }
    for (int i = 0; i < num_moons; i++)
    {
      if (moons_orbit_days[i] == 0.0f) { continue; }
      moons_angle[i] += ( TWO_PI / (moons_orbit_days[i] * (DAY/DT)) );
      while (moons_angle[i] >= TWO_PI)
      {
        moons_angle[i] -= TWO_PI;
      }
    }

    calculate_pos();
  }

  void do_rotation()
  {
    orientation += rotation_dir * (TWO_PI / (day_length * (DAY / DT)));
    while (orientation >= TWO_PI or orientation <= -TWO_PI)
    {
      num_days++;
      orientation -= (rotation_dir * TWO_PI);
    }
  }
  
  void add_moon(float moon_radius, float moon_distance, float days_to_orbit, std::string moon_name)
  {
    for (int i = 0; i < num_moons; i++)
    {
      if (moons_radius[i] <= 0.00f)
      {
        moons_radius[i]     = moon_radius;
        moons_distance[i]   = moon_distance + radius;
        moons_orbit_days[i] = days_to_orbit;
        moons_names[i]      = moon_name;
        break;
      }
    }
  }
};


Planet* planets = NULL;


static void build_scene()
{
  num_planets = 8;
  planets = new Planet[num_planets];


  //Mercury
  planets[0] = Planet(mercury_radius, 0.39f, 0, 1408.0f);
  planets[0].days_to_orbit = 87.97f;
  planets[0].set_color_hex("1a1a1a");
  planets[0].name = "Mercury";

  //Venus
  planets[1] = Planet(venus_radius, 0.72f, 0, 5832.0f);
  planets[1].days_to_orbit = 224.7f;
  planets[1].set_color_hex("e6e6e6");
  planets[1].name = "Venus";
  planets[1].rotation_dir = -1.0f;

  // Earth
  planets[2] = Planet(earth_radius, 1.0f, 1, 24.0f);
  planets[2].days_to_orbit = 365.256365f;
  planets[2].set_color_hex("2f6a69");
  planets[2].add_moon(1.0f, 5.0f, 27.3f, "The Moon!");
  planets[2].name = "Earth";

  // Mars
  planets[3] = Planet(mars_radius, 1.52f, 0, 25.0f);
  planets[3].days_to_orbit = 686.93f;
  planets[3].set_color_hex("993d00");
  planets[3].name = "Mars";

  // Jupiter
  planets[4] = Planet(jupiter_radius, 5.2f, 0, 10.0f);
  planets[4].days_to_orbit = (11.86f * 365.0f);
  planets[4].set_color_hex("b07f35");
  planets[4].name = "Jupiter";

  // Saturn
  planets[5] = Planet(saturn_radius, 9.54f, 0, 11.0f);
  planets[5].days_to_orbit = (29.42f * 365.0f);
  planets[5].set_color_hex("b08f36");
  planets[5].name = "Saturn";

  // Uranus
  planets[6] = Planet(uranus_radius, 19.2f, 0, 17.0f);
  planets[6].days_to_orbit = (83.75f * 365.0f);
  planets[6].set_color_hex("5580aa");
  planets[6].name = "Uranus";

  // Neptune
  planets[7] = Planet(neptune_radius, 30.06f, 0, 16.0f);
  planets[7].days_to_orbit = (163.72f * 365.0f);
  planets[7].set_color_hex("366896");
  planets[7].name = "Neptune";

  num_objects += num_planets;
  for (int i = 0; i < num_planets; i++)
  {
    //num_objects += planets[i].num_moons;
  }
}

/*
Planet    Day Length
Mercury 	1,408 hours
Venus 	  5,832 hours
Earth 	  24 hours
Mars 	    25 hours
Jupiter 	10 hours
Saturn 	  11 hours
Uranus 	  17 hours
Neptune 	16 hours
*/

/* AU distances of planets from sun:
Mercury 0.39
Venus 0.72
Earth 1
Mars 1.52
Jupiter 5.2
Saturn 9.54
Uranus 19.2
Neptune 30.06
*/

/* Time to rotate around the sun:
Mercury:  87.97 days
Venus:    224.7 days
Earth:    365.256365 days
Mars:     686.93 days
Jupiter:  11.86 years
Saturn:   29.42 years
Uranus:   83.75 years
Neptune:  163.72 years
Pluto:    247.92 years
*/

/* Planet colors in hex
Mercury #1a1a1a Yes it is really that dark
Venus #e6e6e6 or perhaps a bit darker
Earth tricky as it is a mix of colors, and changes over the year seems to average out as about #2f6a69
Mars #993d00
Jupiter #b07f35
Saturn #b08f36
Uranus #5580aa
Neptune #366896
*/


static void draw_orbit_str()
{
  std::string counts = "Planet Orbit Counts:";
  draw_centered_bitmap(counts, -(star_radius * 2), star_radius * 2, camera_pos[2]);
  for (int i = 0; i < num_planets; i++)
  {
    //counts += planets[i].name + ": " + std::to_string(planets[i].num_orbits) + '\n';
    draw_centered_bitmap(planets[i].name + ": " + std::to_string(planets[i].num_orbits), -(star_radius * 4), (star_radius * 2) - (13 * (i+1)), camera_pos[2]);
  }
  /*
  for (int i = 0; i < (num_planets + 2); i++)
  {
    counts += '\n';
  }
  */

  //return counts;
}

static std::string get_stats_str()
{
  int int_day = floor(current_day);
  int field_w = 10;
  
  std::string line = "";
  for (int i = 0; i < (field_w * 4) + 4; i++)
  {
    line += '-';
  }

  std::string result = 
      line
    + fill_str(
        "\n| Days Simulated: "  + std::to_string(int_day / 365) + " Years, " + std::to_string(int_day % 365) + " Days",
        (field_w * 4) + 4,
        ' ')
    + " |\n";

  result +=
    "| "
    + fill_str("NAME",    field_w, ' ')
    + fill_str("YEARS",   field_w, ' ')
    + fill_str("ORBITS",  field_w, ' ')
    + fill_str("DAYS",    field_w, ' ')
    + " |\n";

  //std::string counts  = "\nPlanet Orbit Counts:\n";
  //std::string days    = "\nPlanet Days Elapsed:\n";
  for (int i = 0; i < num_planets; i++)
  {
    //counts  += planets[i].name + ": " + std::to_string(planets[i].num_orbits) + '\n';
    //days    += planets[i].name + ": " + std::to_string(planets[i].num_days)   + '\n';

    /*result += fill_str(planets[i].name + ' ', field_w, '-');
    float years_fl = float(planets[i].num_days) / float(planets[i].days_to_orbit);
    result += "Years:  "    + fill_str(std::to_string(years_fl), field_w, ' ');
    result += " | Orbits: " + fill_str()*/
    
    float years_fl = float(planets[i].num_days) / float(planets[i].days_to_orbit);
    result +=
      "| "
      + fill_str(planets[i].name,                       field_w, ' ')
      + fill_str(std::to_string(years_fl),              field_w, ' ')
      + fill_str(std::to_string(planets[i].num_orbits), field_w, ' ')
      + fill_str(std::to_string(planets[i].num_days),   field_w, ' ')
      + " |\n";
  }
  result += line + '\n';
  return (result);
}







static void display_scene()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  if (current_object == 0)
  {
    camera_pos[0]     = DEFAULT_POS[0] + d_camera_pos[0];
    camera_pos[1]     = DEFAULT_POS[1] + d_camera_pos[1];
    camera_pos[2]     = DEFAULT_POS[2] + d_camera_pos[2];
    camera_target[0]  = DEFAULT_POS[0] + d_camera_pos[0];
    camera_target[1]  = DEFAULT_POS[1] + d_camera_pos[1];
    camera_target[2]  = 0.0f;
  }
  else
  {
    camera_pos[0]     = planets[current_object - 1].pos[0] + d_camera_pos[0];
    camera_pos[1]     = planets[current_object - 1].pos[1] + d_camera_pos[1];
    camera_pos[2]     = planets[current_object - 1].pos[2] + ( planets[current_object - 1].radius * 4.0f ) + d_camera_pos[2];
    camera_target[0]  = planets[current_object - 1].pos[0];
    camera_target[1]  = planets[current_object - 1].pos[1];
    camera_target[2]  = planets[current_object - 1].pos[2];
  }
  

  gluLookAt(
    camera_pos[0],    camera_pos[1],    camera_pos[2],
    camera_target[0], camera_target[1], camera_target[2],
    0.0,              1.0,              0.0
  );
  
  //glRotatef(x_orientation, 1.0f, 0.0f, 0.0f);

  int day_int = floor(current_day);
  int year = day_int / 365;
  int day  = day_int % 365;
  std::string time_msg = "Year " + std::to_string(year) + " | Day " + std::to_string(day);
  //std::string day_msg = "Day " + std::to_string(current_day);
  draw_centered_bitmap(time_msg, 0.0f, star_radius * 1.5f, camera_pos[2]);
  std::string orientation_msg = "X Orientation: " + std::to_string(x_orientation);
  //draw_centered_bitmap(orientation_msg, CENTER_X, canvas_Height - 39);
  //std::cout << day_msg << '\r';
  //draw_centered_bitmap(get_orbit_str(), -(star_radius * 2), star_radius * 2);

  // LAST WORKED:
  if (current_object == 0)
  {
    glRotatef(x_orientation, 1.0f, 0.0f, 0.0f);
  }

  // Draw star
  glColor3fv(star_color);
  glPushMatrix();
  //glRotatef(x_orientation, 1.0f, 0.0f, 0.0f);
  glTranslatef(star_pos[0], star_pos[1], star_pos[2]);
  //glRotatef(x_orientation, 1.0f, 0.0f, 0.0f);
  glutWireSphere(star_radius, NUM_SLICES, NUM_STACKS);
  if      (sphere_mode == "WIRE")   { glutWireSphere(star_radius, NUM_SLICES, NUM_STACKS); }
  else if (sphere_mode == "SOLID")  { glutSolidSphere(star_radius, NUM_SLICES, NUM_STACKS); }

  glPopMatrix();

  for (int i = 0; i < num_planets; i++)
  {
    planets[i].draw();
  }
  
  //draw_orbit_str();
  glFinish();
}






static void do_movements(int id)
{
  for (int i = 0; i < num_planets; i++)
  {
    planets[i].do_orbit();
    planets[i].do_rotation();
  }
  current_day += (DT/DAY);
  glutTimerFunc(DT, do_movements, 0);
}

static void get_keys(unsigned char key, int x, int y)
{
  if (std::tolower(key) == 'n')
  {
    bool draw = true;
    if (planets[0].draw_name == true) { draw = false; }
    for (int i = 0; i < num_planets; i++)
    {
      planets[i].draw_name = draw;
    }
  }
  if (std::tolower(key) == 'o')
  {
    bool draw = false;
    if (planets[0].draw_orbit == false) { draw = true; }
    for (int i = 0; i < num_planets; i++)
    {
      planets[i].draw_orbit = draw;
    }
  }
  float speed = movement_speed;
  if (current_object != 0) { speed = planets[current_object - 1].radius / 2.0f; }
  if (std::tolower(key) == 'w') { d_camera_pos[1] += speed; }
  if (std::tolower(key) == 'a') { d_camera_pos[0] -= speed; }
  if (std::tolower(key) == 's') { d_camera_pos[1] -= speed; }
  if (std::tolower(key) == 'd') { d_camera_pos[0] += speed; }

  if (std::tolower(key) == 'o')
  {
    if (current_object == 0)  { d_camera_pos[2] -= 50; }
    else                      { d_camera_pos[2] -= (planets[current_object-1].radius / 2.0f); }
  }
  if (std::tolower(key) == 'k')
  {
    if (current_object == 0)  { d_camera_pos[2] += 50; }
    else                      { d_camera_pos[2] += (planets[current_object-1].radius / 2.0f); }
  }

  if (std::tolower(key) == 'm')
  {
    if      (sphere_mode == "WIRE")   { sphere_mode = "SOLID"; }
    else if (sphere_mode == "SOLID")  { sphere_mode = "WIRE"; }
  }

  if (std::tolower(key) == 'i')
  {
    std::cout << get_stats_str();
  }
}

static void get_special_keys(int key, int x, int y)
{
  if (key == GLUT_KEY_DOWN)
  {
    x_orientation += 0.5;
    while (x_orientation >= 360.0f)
    {
      x_orientation -= 360.0f;
    }
  }
  if (key == GLUT_KEY_UP)
  {
    x_orientation -= 0.5;
    while (x_orientation <= -360.0f)
    {
      x_orientation += 360.0f;
    }
  }
  
  if (key == GLUT_KEY_LEFT)
  {
    if (current_object == 0) { d_camera_pos[0] = 0.0f; d_camera_pos[1] = 0.0f; d_camera_pos[2] = 0.0f; }
    current_object -= 1;
    if (current_object < 0) { current_object = num_objects - 1; }
    if (current_object == 0) { d_camera_pos[0] = 0.0f; d_camera_pos[1] = 0.0f; d_camera_pos[2] = 0.0f; }
  }
  if (key == GLUT_KEY_RIGHT)
  {
    if (current_object == 0) { d_camera_pos[0] = 0.0f; d_camera_pos[1] = 0.0f; d_camera_pos[2] = 0.0f; }
    current_object += 1;
    if (current_object >= num_objects) { current_object = 0; }
    if (current_object == 0) { d_camera_pos[0] = 0.0f; d_camera_pos[1] = 0.0f; d_camera_pos[2] = 0.0f; }
  }

}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  my_setup(canvas_Width, canvas_Height, canvas_Name);
  build_scene();
  glutDisplayFunc(display_scene);
  glutTimerFunc(DT, do_movements, 0);
  glutKeyboardFunc(get_keys);
  glutSpecialFunc(get_special_keys);

  std::cout << "AU = " << AU << '\n';

  glutIdleFunc([]() {
    glutPostRedisplay();
    });

  glutMainLoop();

  return 0;
}