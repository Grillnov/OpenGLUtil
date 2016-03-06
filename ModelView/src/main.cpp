//#include "../demos/Fountain.hpp"
#include "../demos/Phong.hpp"

int main(int argc, char** argv)
{
    GLApplication * app = new Phong();
    app->CreateWindow("Demo", 1024, 768);
    app->RunMainLoop();
    delete app;
    
    return 0;
}
