#ifndef OBJECT_H
#define OBJECT_H

#include <Eigen/Dense>
#include <SFML/Graphics.hpp>

#include <fstream> //for reading/writing files
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <Eigen/Core>

/*
We work in a right-handed coordinate system,
triangles are listed in counterclockwise order when viewed from the outside
*/

class Object
{
private:
    int nodes_mat_length;
    int num_of_nodes;
    Eigen::Matrix<float, -1, 4> nodes;      //nx4 matrix

    int triangles_mat_length;
    int num_of_triangles;
    Eigen::Matrix<int, -1, 3> triangles;      //nx3 matrix

    //normals_mat_length is always equal to nodes_mat_length
    Eigen::Matrix<float, -1, 3> normals;       //nx3 matrix

public:
    int numNodes();
    void setNodeMatSize(int s);
    Eigen::Matrix<float, -1, 4> getNodes();      //nx4 matrix

    int numTriangles();
    void setTriangleMatSize(int s);
    Eigen::Matrix<int, -1, 3> getTriangles();      //nx3 matrix

    sf::Color base_color;

    bool has_normals;
    Eigen::Matrix<float, -1, 4> getNormals();    //nx3 matrix

    bool has_texture_map;
    Eigen::Matrix<float, -1, 2> uv_coords;    // UV-coordinates
    sf::Image texture_map;

    void addNode (float x, float y, float z);
    void addNode (int n, float x, float y, float z);
    void addNode (float x, float y, float z, float nx, float ny, float nz);
    void addNode (int n, float x, float y, float z, float nx, float ny, float nz);
    void addNormal (int n, float nx, float ny, float nz);
    void addTriangle (int n1, int n2, int n3);

    void scale (float cx, float cy, float cz);
    void generateNormals();

    Object();       //create an empty object with no nodes
    void load(std::string obj_name);
    void save(std::string obj_name);
    void load_from_old(std::string filename, bool detached);   //create object from a txt file
};


class Sphere: public Object
{
public:
    Sphere (float radius, int num_longtitudes, int num_latitudes);
};

#endif // OBJECT_H
