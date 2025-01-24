#include "Angel.h"
#include "Sphere.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

Sphere Sun(0.3);        // The biggest value
Sphere Mercury(0.05);
Sphere Venus(0.06);
Sphere Earth(0.07);
Sphere Mars(0.045);
Sphere Jupiter(0.21);
Sphere Saturn(0.14);
Sphere Uranus(0.105);
Sphere Neptune(0.100);
Sphere Sky(10.0);

GLuint vao, vposition;
GLuint vbo1, ibo1;
GLuint vbo2, ibo2;
GLuint vbo3, ibo3;
GLuint vbo4, ibo4;
GLuint vbo5, ibo5;
GLuint vbo6, ibo6;
GLuint vbo7, ibo7;
GLuint vbo8, ibo8;
GLuint vbo9, ibo9;
GLuint vbo10, ibo10;


GLuint colorAttrib, transAttrib, projectionAttrib, viewAttrib, cameraUniform, viewUniform, vertexPosition, vNormal;
GLuint program, vTexture, suntexture, earthtexture, mercurytexture, venustexture,
marstexture, jupitertexture, saturntexture, uranustexture, neptunetexture;
GLuint skyboxTexture, skyboxVBO, skyboxVAO, skyboxIBO;
const float RING_INNER_RADIUS = 0.16f;  // Inner radius
const float RING_OUTER_RADIUS = 0.25f; // Outer radius
const int RING_POINT_COUNT = 1000;      // Number of points in the ring

glm::vec3 cameraPos, cameraTarget, cameraDirection, cameraRight, cameraUp, cameraFront;
glm::mat4 view, projection;
float speed = 0.1f, yaw = -90.0f;
GLuint ringVBO; // Buffer for ring points

// generate ring vertices
std::vector<glm::vec3> generateRingPoints() {
    std::vector<glm::vec3> ringPoints;
    for (int i = 0; i < RING_POINT_COUNT; ++i) {
        float radius = RING_INNER_RADIUS + static_cast<float>(rand()) / RAND_MAX * (RING_OUTER_RADIUS - RING_INNER_RADIUS);
        float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        ringPoints.push_back(glm::vec3(x, 0.0f, z)); // y = 0
    }
    return ringPoints;
}

void initRing() {
    std::vector<glm::vec3> ringPoints = generateRingPoints();

    // Create and bind buffer for ring points
    glGenBuffers(1, &ringVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ringVBO);
    glBufferData(GL_ARRAY_BUFFER, ringPoints.size() * sizeof(glm::vec3), &ringPoints[0], GL_STATIC_DRAW);
}

void drawRing() {
    glBindBuffer(GL_ARRAY_BUFFER, ringVBO);
    glVertexAttribPointer(vposition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(vposition);

    // Set ring color (e.g., light gray)
    glUniform3f(glGetUniformLocation(program, "Color"), 0.8f, 0.8f, 0.8f);

    // Draw points
    glDrawArrays(GL_POINTS, 0, RING_POINT_COUNT);
}
/////////////////////////////////////////

// buffers
void createBufferObject(GLuint& vbo, GLuint& ibo, const Sphere& object) {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, object.getInterleavedVertexSize(), object.getInterleavedVertices(), GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.getIndexSize(), object.getIndices(), GL_STATIC_DRAW);
}
void initTexture()
{
    stbi_set_flip_vertically_on_load(true);

    glGenTextures(1, &suntexture);
    glBindTexture(GL_TEXTURE_2D, suntexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    unsigned char* data1 = stbi_load("sun.jpg", &width, &height, &channels, 0);

    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data1);

    glGenTextures(1, &earthtexture);
    glBindTexture(GL_TEXTURE_2D, earthtexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data2 = stbi_load("2k_earth_daymap.jpg", &width, &height, &channels, 0);

    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data2);


    glGenTextures(1, &mercurytexture);
    glBindTexture(GL_TEXTURE_2D, mercurytexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data3 = stbi_load("2k_mercury.jpg", &width, &height, &channels, 0);

    if (data3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data3);


    glGenTextures(1, &venustexture);
    glBindTexture(GL_TEXTURE_2D, venustexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data4 = stbi_load("2k_venus_surface.jpg", &width, &height, &channels, 0);

    if (data4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data4);


    glGenTextures(1, &marstexture);
    glBindTexture(GL_TEXTURE_2D, marstexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data5 = stbi_load("2k_mars.jpg", &width, &height, &channels, 0);

    if (data5)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data5);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data5);


    glGenTextures(1, &jupitertexture);
    glBindTexture(GL_TEXTURE_2D, jupitertexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data6 = stbi_load("2k_jupiter.jpg", &width, &height, &channels, 0);

    if (data6)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data6);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data6);


    glGenTextures(1, &saturntexture);
    glBindTexture(GL_TEXTURE_2D, saturntexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data7 = stbi_load("2k_saturn.jpg", &width, &height, &channels, 0);

    if (data7)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data7);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data7);


    glGenTextures(1, &uranustexture);
    glBindTexture(GL_TEXTURE_2D, uranustexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data8 = stbi_load("2k_uranus.jpg", &width, &height, &channels, 0);

    if (data8)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data8);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data8);

    glGenTextures(1, &neptunetexture);
    glBindTexture(GL_TEXTURE_2D, neptunetexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data9 = stbi_load("2k_neptune.jpg", &width, &height, &channels, 0);

    if (data9)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data9);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data9);


    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &skyboxTexture);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data10 = stbi_load("2k_stars.jpg", &width, &height, &channels, 0);

    if (data10) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data10);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data10);
}

void init() {

    // Create and bind the Vertex Array Object (VAO)
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create buffers for each object
    createBufferObject(skyboxVBO, skyboxIBO, Sky);
    createBufferObject(vbo1, ibo1, Sun);
    createBufferObject(vbo2, ibo2, Mercury);
    createBufferObject(vbo3, ibo3, Venus);
    createBufferObject(vbo4, ibo4, Earth);
    createBufferObject(vbo5, ibo5, Mars);
    createBufferObject(vbo6, ibo6, Jupiter);
    createBufferObject(vbo7, ibo7, Saturn);
    createBufferObject(vbo8, ibo8, Uranus);
    createBufferObject(vbo9, ibo9, Neptune);

    // Initialize shaders
    program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // Get attribute locations from the shader
    vposition = glGetAttribLocation(program, "aPos");
    vNormal = glGetAttribLocation(program, "aNormal");

    glEnableVertexAttribArray(vposition);
    glVertexAttribPointer(vposition, 3, GL_FLOAT, GL_FALSE, Sun.getInterleavedStride(), (void*)0);

    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, Sun.getInterleavedStride(), (void*)(3 * sizeof(float)));

    // Initialize camera and projection
    cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    projection = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 20.0f);

    // Send the projection matrix to the shader
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Set light properties
    glUniform3f(glGetUniformLocation(program, "light.position"), 0.0f, 0.0f, 1.0f); // Assuming Sun at origin
    glUniform3f(glGetUniformLocation(program, "light.ambient"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(program, "light.specular"), 1.0f, 1.0f, 1.0f);

    // Set material properties
    glUniform3f(glGetUniformLocation(program, "material.ambient"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(program, "material.diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(program, "material.specular"), 0.5f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(program, "material.shininess"), 40.0f);

    // Set sun color (yellow)
    vTexture = glGetAttribLocation(program, "vertexTexture");
    glEnableVertexAttribArray(vTexture);
    glEnableVertexAttribArray(vposition);

    initTexture();
    // Enable depth testing and set clear color
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    initRing();
}


void drawPlanet(GLuint vbo, GLuint ibo, float orbitSpeed, float selfRotationSpeed, float distanceFromSun, glm::vec3 color, bool ring, GLuint texture, bool light) {

    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;  // Get the elapsed time
    // Orbit and self-rotation
    glm::mat4 orbit = glm::rotate(glm::mat4(1.0f), angle * orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis
    orbit = glm::translate(orbit, glm::vec3(distanceFromSun, 0.0f, 0.0f));  // Distance from Sun
    glm::mat4 selfRotation = glm::rotate(glm::mat4(1.0f), angle * selfRotationSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 transform = orbit * selfRotation;
    if (!light)
    {
        glUniform1i(glGetUniformLocation(program, "isSkybox"), 0);
    }
    else
    {
        glUniform1i(glGetUniformLocation(program, "isSkybox"), 1);
    }
    glUniformMatrix4fv(transAttrib, 1, GL_FALSE, glm::value_ptr(transform));
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindTexture(GL_TEXTURE_2D, texture);
    glVertexAttribPointer(vposition, 3, GL_FLOAT, GL_FALSE, Sun.getInterleavedStride(), (void*)0);
    glVertexAttribPointer(vTexture, 2, GL_FLOAT, false, Sun.getInterleavedStride(), (void*)(6 * sizeof(float)));
    glDrawElements(GL_TRIANGLES, Sun.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

    if (ring == true)
    {
        glm::mat4 ringTransform = transform;
        glUniformMatrix4fv(transAttrib, 1, GL_FALSE, glm::value_ptr(ringTransform));
        drawRing();
    }
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 modelV = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelV));

    glBindVertexArray(vao);

    // Set Sun color
    glUniform3f(glGetUniformLocation(program, "Color"), 1.0, 1.0, 1.0);  // Set the color for each planet
    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;  // Get the elapsed time
    glm::mat4 sunRotation = glm::rotate(glm::mat4(1.0f), angle * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around its Z-axis
    glUniformMatrix4fv(transAttrib, 1, GL_FALSE, glm::value_ptr(sunRotation));
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo1);
    glBindTexture(GL_TEXTURE_2D, suntexture);
    glVertexAttribPointer(vposition, 3, GL_FLOAT, GL_FALSE, Sun.getInterleavedStride(), (void*)0);
    glVertexAttribPointer(vTexture, 2, GL_FLOAT, false, Sun.getInterleavedStride(), (void*)(6 * sizeof(float)));
    glDrawElements(GL_TRIANGLES, Sun.getIndexCount(), GL_UNSIGNED_INT, 0);

    // Set another planet's color (example)
    drawPlanet(vbo2, ibo2, 0.8f, 2.0f, 0.4f, glm::vec3(0.5490f, 0.5490f, 0.5804f), false, mercurytexture, false);  // Mercury
    drawPlanet(vbo3, ibo3, 0.7f, 2.0f, 0.6f, glm::vec3(0.8275f, 0.6471f, 0.4039f), false, venustexture, false);  // Venus
    drawPlanet(vbo4, ibo4, 0.6f, 2.0f, 0.8f, glm::vec3(0.3098f, 0.2980f, 0.6902f), false, earthtexture, false);  // Earth
    drawPlanet(vbo5, ibo5, 0.5f, 2.0f, 1.0f, glm::vec3(0.7569f, 0.2667f, 0.0549f), false,marstexture, false);  // Mars
    drawPlanet(vbo6, ibo6, 0.4f, 2.0f, 1.2f, glm::vec3(0.7569f, 0.4902f, 0.4196f), false,jupitertexture, false);  // Jupiter
    drawPlanet(vbo7, ibo7, 0.3f, 2.0f, 1.4f, glm::vec3(0.9882f, 0.9333f, 0.6784f), true,saturntexture, false);  // Saturn
    drawPlanet(vbo8, ibo8, 0.2f, 2.0f, 1.6f, glm::vec3(0.3843f, 0.6824f, 0.9059f), false,uranustexture, false);  // Uranus
    drawPlanet(vbo9, ibo9, 0.1f, 2.0f, 1.8f, glm::vec3(0.2392f, 0.3686f, 0.9765f), false,neptunetexture, false);  // Neptune
    drawPlanet(skyboxVBO, skyboxIBO, 0.05f, 0.05f, 1.8f, glm::vec3(0.2392f, 0.3686f, 0.9765f), false, skyboxTexture, true);  // Neptune



    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 033:
        exit(EXIT_SUCCESS);
        break;
    case 'w':
        cameraPos += speed * cameraFront;
        break;
    case 's':
        cameraPos -= speed * cameraFront;
        break;
    case 'a':
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
        break;
    case 'd':
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
        break;
    case 'q':
        yaw -= 1.0f;
        cameraDirection.x = glm::cos(glm::radians(yaw));
        cameraDirection.z = glm::sin(glm::radians(yaw));
        cameraFront = glm::normalize(cameraDirection);
        break;
    case 'e':
        yaw += 1.0f;
        cameraDirection.x = glm::cos(glm::radians(yaw));
        cameraDirection.z = glm::sin(glm::radians(yaw));
        cameraFront = glm::normalize(cameraDirection);
        break;
    }
}

void idle()
{
    glutPostRedisplay();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1000, 800);
    glutCreateWindow("3D Solar System");

    glewInit();
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    //glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}
