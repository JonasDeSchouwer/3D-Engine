#include "Camera.h"

Camera::Camera(int width_, int height_, int x_, int y_, int z_, std::deque<Object> objects)
{
    width = width_;
    height = height_;
    render_texture.create(width, height);
    myObjects = objects;
    background_color = sf::Color (0,211,211);

    zoom_x = 2;
    zoom_y = zoom_x * (double) width/height;
    near_z = 2;
    far_z = 10000;

    x = x_;
    y = y_;
    z = z_;
    speed = 300;
    angle << 0,0,1;     //angle will always be normalized
    maxLookUp = 85*PI/180;
}

Eigen::Matrix4f Camera::getTranslation()
{
    Eigen::Matrix4f M;
    M <<    1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            -x,-y,-z,1;
    return M;
}

void Camera::changePos (float dx, float dy, float dz)
{
    x += dx;
    y += dy;
    z += dz;
}

void Camera::strafe (float dx, float dy, float dz)
{
    float n = sqrt(angle[0]*angle[0] + angle[2]*angle[2]);
    x += -dx*angle[2]/n + dz*angle[0]/n;
    y += dy;
    z += dx*angle[0]/n + dz*angle[2]/n;
}

void Camera::rotateAngle (float rotateToX, float rotateToY)
{
    //rotate to world x
    Eigen::Matrix3f M;
    M << cos(rotateToX), 0, sin(rotateToX),
         0, 1, 0,
         -sin(rotateToX), 0, cos(rotateToX);
    angle = M * angle;

    float n = sqrt(angle[0]*angle[0] + angle[2]*angle[2]);
//    //rotate to camera x
//    Eigen::Vector3f angleX (angle[2]/n, 0, -angle[0]/n);
//    angle = cos(rotateToX) * angle.array() + sin(rotateToX) * angleX.array();

    //rotate to y

    //check if looking too far up or down
    float theta = asin(angle[1]);
    if (rotateToY+theta <= -maxLookUp)  //looking too far down
        {rotateToY = -maxLookUp-theta;}
    if (rotateToY+theta >= maxLookUp)   //looking too far up
        {rotateToY = maxLookUp-theta;}
    n = sqrt(angle[0]*angle[0] + angle[2]*angle[2]);
    Eigen::Vector3f angleY (-angle[0]*angle[1]/n, n, -angle[2]*angle[1]/n);
    angle = cos(rotateToY) * angle.array() + sin(rotateToY) * angleY.array();
}

Eigen::Matrix4f Camera::worldSpaceToCameraSpace()
{
    angle.normalize();
    float ax = angle[0];
    float ay = angle[1];
    float az = angle[2];
    float n = sqrt(ax*ax + az*az);

    //row1: angle_x_axis (perpendicular to angle, in the world xz-plane)
    //row2: angle_y_axis (perpendicular to the other two, always pointing up)
    //row3: angle_z_axis (parallel to angle)

    //row2 is normalized, as (ax2*ay2 + az2*ay2 + (ax2+az2)^2)/(ax2+az2) = (ax2+ay2+az2)*(ax2+az2)/(ax2+az2)
    Eigen::Matrix4f M;
    M <<    -az/n,  0,      ax/n,   0,
            -ax*ay/n, n,    -az*ay/n, 0,
            ax,     ay,     az,     0,
            0,      0,      0,      1;

    return getTranslation() * M.inverse();

//    M <<    angle[2]/n, 0, -angle[0]/n, 0,
//            -angle[0]*angle[1]/n, n, -angle[2]*angle[1]/n, 0,
//            angle[0], angle[1], angle[2], 0,
//            0,0,0,1;
//    return getTranslation() * M.inverse();
}

Eigen::Matrix4f Camera::cameraSpaceToClipSpace()
{
    /*
    put z in the w-coordinate and prepare the other coordinates for clipping:
    a point is in the view frustrum if
        -w <= x <= w
        -w <= y <= w
        -w <= z <= w
    */

    //0,0,f,1     ->  0,0,f,f
    //0,0,n,1     ->  0,0,-n,n
    //1,0,zx,1    ->  zx,0,.,zx
    //0,1,zy,1    ->  0,zy,.,zy
    Eigen::Matrix4f M;
    M <<    zoom_x,0,0,0,
            0,zoom_y,0,0,
            0,0, (far_z+near_z)/(far_z-near_z), 1,
            0,0, -2*near_z*far_z/(far_z-near_z), 0;
    return M;
}

void Camera::display()
{
    render_texture.clear(background_color);

    for (unsigned int i=0; i<myObjects.size(); i++)
    {
        Object object = myObjects[i];
        Eigen::MatrixXf camera_space = object.getNodes() * worldSpaceToCameraSpace();
        Eigen::MatrixXf clip_space = camera_space * cameraSpaceToClipSpace();

        Eigen::VectorXf dehomogenizedX = clip_space.col(0).array() / clip_space.col(3).array();
        Eigen::VectorXf dehomogenizedY = clip_space.col(1).array() / clip_space.col(3).array();
        Eigen::VectorXf dehomogenizedZ = clip_space.col(2).array() / clip_space.col(3).array();

        Eigen::VectorXf screenX = (-width/2) * dehomogenizedX.array() + (width/2);
        Eigen::VectorXf screenY = (-height/2) * dehomogenizedY.array() + (height/2);

        //display faces
        for (int j=0; j<object.numTriangles(); j++)
        {
            Eigen::RowVector3i triangle = object.getTriangles().row(j);

            Eigen::Vector3f node0 = camera_space.block<1,3>(triangle[0],0);
            Eigen::Vector3f node1 = camera_space.block<1,3>(triangle[1],0);
            Eigen::Vector3f node2 = camera_space.block<1,3>(triangle[2],0);
            Eigen::Vector3f plane_normal = (node2-node0).cross(node1-node0);        //reverse plane normal
            plane_normal.normalize();
            if (plane_normal[2] > 0)    //do not display if the face is turned away
                {continue;}

            sf::VertexArray vertex_array (sf::TriangleFan, 3);
            for (unsigned int k=0; k<3; k++)
            {
                vertex_array[k].position = sf::Vector2f (screenX[triangle[k]], screenY[triangle[k]]);
                vertex_array[k].color = object.base_color;
            }
            render_texture.draw (vertex_array);
        }

        //display nodes
        for (int j=0; j<object.numNodes(); j++)
        {
            if (dehomogenizedX[j] >= -1 && dehomogenizedX[j] <= 1
                && dehomogenizedY[j] >= -1 && dehomogenizedY[j] <= 1
                && dehomogenizedZ[j] >= -1 && dehomogenizedZ[j] <= 1)
            {
                sf::CircleShape node (4);
                node.setPosition (screenX[j]-2, screenY[j]-2);
                render_texture.draw(node);
            }
        }
        //display node numbers
        sf::Font font;
        if (!font.loadFromFile("arial.ttf"))
        {
            std::cout << "unable to load font" << std::endl;
        }
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(20);

        for (int j=0; j<object.numNodes(); j++)
        {
            text.setString(std::to_string(j));
            text.setPosition (screenX[j] + 4 - text.getLocalBounds().left,
                              screenY[j] + 4 - text.getLocalBounds().top);
            render_texture.draw(text);
        }
    }

    render_texture.display();
}

void Camera::displayCoordinateSystems ()
{
    for (unsigned int i=0; i<myObjects.size(); i++)
    {
        Object object = myObjects[i];
        Eigen::MatrixXf camera_space = object.getNodes() * worldSpaceToCameraSpace();
        Eigen::MatrixXf clip_space = camera_space * cameraSpaceToClipSpace();
        Eigen::VectorXf dehomogenizedX = clip_space.col(0).array() / clip_space.col(3).array();
        Eigen::VectorXf dehomogenizedY = clip_space.col(1).array() / clip_space.col(3).array();

        Eigen::VectorXf screenX = (width/2) * dehomogenizedX.array() + (width/2);
        Eigen::VectorXf screenY = (-height/2) * dehomogenizedY.array() + (height/2);

        std::cout << "\n\n\n\n\n";
        std::cout << "World space:" << std::endl;
        for (int j=0; j<object.numNodes(); j++)
        {
            std::cout << "Node " << j << ":   " << object.getNodes()(j,0) << "   " << object.getNodes()(j,1) << "   " << object.getNodes()(j,2) << std::endl;
        }

        std::cout << "\n\n\n\n\n";
        std::cout << "Camera space:" << std::endl;
        for (int j=0; j<object.numNodes(); j++)
        {
            std::cout << "Node " << j << ":   " << camera_space(j,0) << "   " << camera_space(j,1) << "   " << camera_space(j,2) << std::endl;
        }

        std::cout << "\n\n\n\n\n";
        std::cout << "Clip space:" << std::endl;
        for (int j=0; j<object.numNodes(); j++)
        {
            std::cout << "Node " << j << ":   " << clip_space(j,0) << "   " << clip_space(j,1) << "   " << clip_space(j,2) << std::endl;
        }

        std::cout << "\n\n\n\n\n";
        std::cout << "Screen space:" << std::endl;
        for (int j=0; j<object.numNodes(); j++)
        {
            std::cout << "Node " << j << ":   " << screenX[j] << "   " << screenY[j] << std::endl;
        }
    }
}
