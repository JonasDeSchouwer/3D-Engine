#include "Object.h"

#include "windows.h"
#include  <sys/stat.h> //to check if a file or directory exists
#include <string>
#include <cstring>

Object::Object()
{
    nodes_mat_length = 0;
    num_of_nodes = 0;
    triangles_mat_length = 0;
    num_of_triangles = 0;

    base_color = sf::Color::Blue;

    has_normals = false;
    has_texture_map = false;
}


int Object::numNodes()
{
    return num_of_nodes;
}

void Object::setNodeMatSize(int s)
{
    assert (s >= 0);
    if (s < num_of_nodes) {
        std::cout << "WARNING: removing nodes by resize" << std::endl;
        num_of_nodes = s;
    }

    nodes.conservativeResize(s, 4);
    normals.conservativeResize(s, 3);
}

Eigen::Matrix<float,-1,4> Object::getNodes()
{
    return nodes.topLeftCorner(num_of_nodes, 4);
}

void Object::addNode (float x, float y, float z)
{
    addNode(num_of_nodes, x, y, z);
}

void Object::addNode (int node_number, float x, float y, float z)
{
    assert (node_number >= 0);
    if (node_number >= nodes_mat_length)
    {
        setNodeMatSize(node_number+1);
    }

    nodes(node_number, 0) = x;
    nodes(node_number, 1) = y;
    nodes(node_number, 2) = z;
    nodes(node_number, 3) = 1;

    num_of_nodes += 1;
}

void Object::addNode (float x, float y, float z, float nx, float ny, float nz)
{
    addNode(num_of_nodes, x, y, z, nx, ny, nz);
}

void Object::addNode (int node_number, float x, float y, float z, float nx, float ny, float nz)
{
    addNode (node_number, x, y, z);
    addNormal (node_number, nx, ny, nz);
}

void Object::addNormal (int node_number, float nx, float ny, float nz)
{
    assert (node_number >= 0);
    assert (node_number < num_of_nodes);

    normals(node_number, 0) = nx;
    normals(node_number, 1) = ny;
    normals(node_number, 2) = nz;
}

int Object::numTriangles()
{
    return num_of_triangles;
}

void Object::setTriangleMatSize(int s)
{
    assert (s >= 0);
    if (s < num_of_triangles) {
        std::cout << "WARNING: removing triangles by resize" << std::endl;
        num_of_triangles = s;
    }

    triangles.conservativeResize(s, 3);
}

Eigen::Matrix<int,-1,3> Object::getTriangles()
{
    return triangles.topLeftCorner(num_of_triangles, 3);
}


void Object::addTriangle (int n1, int n2, int n3)
{
//    std::cout << std::max(n1, n2, n3) << std::endl;;
//    if (std::max(n1, n2, n3) >= numNodes()) {
//        std::cout << "WARNING: adding non-existing nodes to a triangle: " << n1 << "," << n2 << "," << n3 << " with num_nodes = " << num_of_nodes << std::endl;
//    }


    int triangle_number = num_of_triangles;
    num_of_triangles += 1;

    if (triangle_number >= triangles_mat_length)
    {
        setTriangleMatSize(triangle_number+1);
    }

    triangles(triangle_number, 0) = n1;
    triangles(triangle_number, 1) = n2;
    triangles(triangle_number, 2) = n3;
}


void Object::scale(float cx, float cy, float cz)
{
    nodes.col(0) *= cx;
    nodes.col(1) *= cy;
    nodes.col(2) *= cz;
}


void Object::generateNormals()
{
    normals = Eigen::ArrayXXf::Zero(nodes_mat_length, 3);

    // add the contribution of each face normal to the node normal
    for (int t=0; t<num_of_triangles; t++) {
        Eigen::RowVector3i triangle = getTriangles().row(t);

        Eigen::RowVector3f node0 = getNodes().block<1,3>(triangle[0],0);
        Eigen::RowVector3f node1 = getNodes().block<1,3>(triangle[1],0);
        Eigen::RowVector3f node2 = getNodes().block<1,3>(triangle[2],0);
        Eigen::RowVector3f plane_normal = (node1-node0).cross(node2-node0);
        plane_normal.normalize();

        normals.row(triangle[0]) += plane_normal;
        normals.row(triangle[1]) += plane_normal;
        normals.row(triangle[2]) += plane_normal;
    }

    // normalize every normal
    normals.rowwise().normalize();
}


void Object::load (std::string obj_name)
{
    //std::string filename = "../../../objects/" + obj_name + "/" + obj_name + ".txt";
    std::string filename = "C:/Users/jonas/OneDrive/informatica/c++/3D engine - reboot 2/objects/" + obj_name + "/" + obj_name + ".txt";
    std::ifstream myFile (filename.c_str());
    if (!myFile.is_open())
        {std::cout << "unable to open file " << filename << std::endl;}

    std::string header;

    //base_color
    myFile >> header;
    if (header != "<base_color>")
        {std::cout << "header is not '<base_color>', header: " + header;}
    int r,g,b;
    myFile >> r >> g >> b;
    base_color = sf::Color (r,g,b,255);

    //nodes
    myFile >> header;
    if (header != "<nodes>")
        {std::cout << "header is not '<nodes>', header: " + header;}
    myFile >> nodes_mat_length;
    nodes.conservativeResize(nodes_mat_length, 4);
    float x,y,z;
    for (int i=0; i<num_of_nodes; i++)
    {
        myFile >> x >> y >> z;
        addNode(x,y,z);
        std::cout << "added node " << i << ": " << x << " " << y << " " << z << std::endl;
    }

    //triangles
    myFile >> header;
    if (header != "<triangles>")
        {std::cout << "header is not '<triangles>', header: " + header;}
    myFile >> triangles_mat_length;
    triangles.conservativeResize(triangles_mat_length, 3);
    int n1, n2, n3;
    for (int i=0; i<num_of_triangles; i++)
    {
        myFile >> n1 >> n2 >> n3;
        addTriangle(n1,n2,n3);
        std::cout << "added triangle " << i << ": " << n1 << " " << n2 << " " << n3 << std::endl;
    }

    //normals
    myFile >> header;
    if (header != "<normals>")
        {std::cout << "header is not '<normals>', header: " + header;}
    int num_of_normals;
    myFile >> num_of_normals;
    if (num_of_normals == 0)
    {
        has_normals = false;
        std::cout << "no normals" << std::endl;
    }
    else if (num_of_normals == num_of_nodes)
    {
        has_normals = true;
        normals.conservativeResize (num_of_nodes, 3);
        for (int i=0; i<num_of_nodes; i++)
        {
            myFile >> x >> y >> z;
            normals(i, 0) = x;
            normals(i, 1) = y;
            normals(i, 2) = z;
        }
    }
    else
    {
        std::cout << "number of normals (" << num_of_normals << ") and number of nodes (" << num_of_nodes << ") does not match." << std::endl;
    }

    //uv_coords
    myFile >> header;
    if (header != "<uv_coords>")
        {std::cout << "header is not '<uv_coords>', header: " + header << std::endl;}
    int num_of_coords;
    myFile >> num_of_coords;
    if (num_of_coords == 0)
    {
        has_texture_map = false;
        std::cout << "no texture map" << std::endl;
    }
    else if (num_of_coords == num_of_nodes)
    {
        has_texture_map = true;
        uv_coords.conservativeResize (num_of_nodes, 2);
        float u,v;
        for (int i=0; i<num_of_nodes; i++)
        {
            myFile >> u >> v;
            uv_coords(i, 0) = u;
            uv_coords(i, 1) = v;
        }
    }
    else
    {
        std::cout << "number of uv_coords (" << num_of_coords << ") and number of nodes (" << num_of_nodes << ") does not match." << std::endl;
    }

    myFile.close();

    if (has_texture_map)
    {
        std::string filename = "../objects/" + obj_name + "/texture.png";
        if (!texture_map.loadFromFile(filename))
            {std::cout << "failed to load texture map " << filename << std::endl;}
    }
}

void Object::save (std::string obj_name)
{
    //file management
    std::string filename = "C:/Users/jonas/OneDrive/informatica/c++/3D engine - reboot 2/objects/" + obj_name + "/" + obj_name + ".txt";
    struct stat buffer;
    std::string directory_name = "C:/Users/jonas/OneDrive/informatica/c++/3D engine - reboot 2/objects/" + obj_name;
    if (stat(directory_name.c_str(), &buffer) == 0)
        {std::cout << "directory for saving already existed" << std::endl;}
    else if (mkdir(directory_name.c_str()) != 0)
        {std::cout << "failed to create object directory for saving" << std::endl;}
    else {std::cout << "succeeded in creating an object for saving" << std::endl;}

    std::ofstream myFile (filename);

    if (!myFile.is_open())
        {std::cout << "failed to open file " << filename << std::endl;}

    //base_color
    myFile << "<base_color>\n";
    myFile << static_cast<UINT32>(base_color.r) << " " << static_cast<UINT32>(base_color.g) << " " << static_cast<UINT32>(base_color.b) << "\n";
    myFile << "\n";

    //nodes
    myFile << "<nodes> " << num_of_nodes << "\n";
    for (int i=0; i<num_of_nodes; i++)
        {myFile << nodes(i,0) << " " << nodes(i,1) << " " << nodes(i,2) << "\n";}
    myFile << "\n";

    //triangles
    myFile << "<triangles> " << num_of_triangles << "\n";
    for (int i=0; i<num_of_triangles; i++)
        {myFile << triangles(i,0) << " " << triangles(i,1) << " " << triangles(i,2) << "\n";}
    myFile << "\n";

    //normals
    if (has_normals)
    {
        myFile << "<normals> " << num_of_nodes << "\n";
        for (int i=0; i<num_of_nodes; i++)
            {myFile << normals(i,0) << " " << normals(i,1) << " " << normals(i,2) << "\n";}
    }
    else
    {
        myFile << "<normals> " << 0 << "\n";
    }
    myFile << "\n";

    //uv_coords
    if (has_texture_map)
    {
        myFile << "<uv_coords> " << num_of_nodes << "\n";
        for (int i=0; i<num_of_nodes; i++)
            {myFile << uv_coords(i,0) << " " << uv_coords(i,1) << "\n";}
        texture_map.saveToFile("../objects/" + obj_name + "/texture.png");
    }
    else
    {
        myFile << "<uv_coords> " << 0 << "\n";
    }
    myFile << "\n";

    myFile.close();
}

void Object::load_from_old(std::string filename, bool detached=false)
{
    num_of_nodes = 0;
    num_of_triangles = 0;
    base_color = sf::Color::Blue;

    has_normals = false;
    has_texture_map = false;

    std::ifstream myFile (filename.c_str());
    if (!myFile.is_open())
        {std::cout << "unable to open file " << filename.c_str() << std::endl;}

    //nodes
    myFile >> num_of_nodes;
    nodes.conservativeResize(num_of_nodes, 4);
    float x,y,z;
    for (int i=0; i<num_of_nodes; i++)
    {
        myFile >> x >> y >> z;
        addNode(x,y,z);
        std::cout << "added node " << i << ": " << x << " " << y << " " << z << std::endl;
    }

    //edges - just read and ignore
    int num_of_edges;
    myFile >> num_of_edges;
    int node1, node2;
    int r,g,b,a;
    for (int i=0; i<num_of_edges; i++)
    {
        myFile >> node1 >> node2 >> r >> g >> b >> a;
        sf::Color ecolor (r,g,b,a);
    }

    //faces
    int num_of_faces;
    myFile >> num_of_faces;
    std::string line;
    while (getline(myFile, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::stringstream this_line (line);

        std::vector<int> face_nodes;
        int node;
        while (this_line >> node)
            {face_nodes.push_back(node);}

        //pop face color data
        for (int i=0; i<4; i++)
            {face_nodes.pop_back();}

        int n1,n2,n3;
        n1 = face_nodes[0];
        num_of_triangles += face_nodes.size()-2;
        triangles.conservativeResize(num_of_triangles, 3);
        for (int i=1; i<face_nodes.size(); i++)
        {
            n2 = face_nodes[i];
            n3 = face_nodes[i+1];
            addTriangle(n1,n2,n3);
            Eigen::RowVector3f v21 = nodes(n2, Eigen::seq(0,2)) - nodes(n1, Eigen::seq(0,2));
            Eigen::RowVector3f v31 = nodes(n3, Eigen::seq(0,2)) - nodes(n1, Eigen::seq(0,2));
            Eigen::RowVector3f face_vector = v21.cross(v31);
            Eigen::RowVector3f node_vector;
            node_vector << nodes(n1,0), nodes(n1,1), nodes(n1,2);
            if (face_vector.dot(node_vector) < 0)
                {std::cout << "WARNING: face is oriented towards origin" << std::endl;}
        }
    }

    myFile.close();
}


Sphere::Sphere(float radius, int num_longtitudes, int num_latitudes)
{
    const float PI = 3.141592653;

// nodes
    addNode(0,0,1);

    for (int lat_idx=1; lat_idx<num_latitudes-1; lat_idx++) {
        float phi = lat_idx * PI/num_latitudes;

        for (int long_idx=0; long_idx<num_longtitudes; long_idx++) {
            float theta = long_idx * 2*PI/num_longtitudes;

            addNode(sin(phi)*cos(theta), sin(phi)*sin(theta), cos(phi));
        }
    }

    addNode(0,0,-1);

    scale(radius, radius, radius);


// triangles
    // upper row
    int lat_idx = 1;
    for (int long_idx=0; long_idx<num_longtitudes-1; long_idx++) {
        int node = 1 + num_longtitudes*(lat_idx-1) + long_idx;
        addTriangle(0, node, node+1);
    }
    int long_idx = num_longtitudes-1;
    int node = 1 + num_longtitudes*(lat_idx-1) + long_idx;
    addTriangle(0, num_longtitudes, 1);

    // middle rows
    for (int lat_idx=1; lat_idx<num_latitudes-2; lat_idx++) {
        for (int long_idx=0; long_idx<num_longtitudes-1; long_idx++) {
            int node1 = 1 + num_longtitudes*(lat_idx-1) + long_idx;     //upper left corner of the rectangle
            int node2 = node1+1;                    //upper right
            int node3 = node1+num_longtitudes;      //lower left
            int node4 = node1+num_longtitudes+1;    //lower right
            addTriangle(node1, node3, node2);
            addTriangle(node2, node3, node4);
        }

        //closing the loop
        int long_idx = num_longtitudes-1;
        int node1 = 1 + num_longtitudes*(lat_idx-1) + long_idx;
        int node2 = 1 + num_longtitudes*(lat_idx-1);
        int node3 = node1 + num_longtitudes;
        int node4 = node2 + num_longtitudes;
        addTriangle(node1, node3, node2);
        addTriangle(node2, node3, node4);
    }

    //bottom row
    lat_idx = num_latitudes-2;
    for (int long_idx=0; long_idx<num_longtitudes-1; long_idx++) {
        int node = 1 + num_longtitudes*(lat_idx-1) + long_idx;
        addTriangle(node+1, node, numNodes()-1);
    }
    long_idx = num_longtitudes-1;
    node = 1 + num_longtitudes*(lat_idx-1) + long_idx;
    addTriangle(node+1-num_longtitudes, node, numNodes()-1);


// normals
    for (int n=0; n<numNodes(); n++) {
        Eigen::RowVector4f node = getNodes().row(n);
        node.normalize();
        addNormal(n, node(0), node(1), node(2));
    }
}
