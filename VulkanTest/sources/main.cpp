#include "vulkan.hpp"
int main () {

    glm::vec3 color = {0.0f, 0.0f, 1.0f};

        // if (res1.find (i) != res1.end())
        //     color = {1.0f, 0.0f, 0.0f};

    // surface_ S (T[i].a, T[i].b, T[i].c);

    glm::vec3 normal (1, 1, 1);

    dblCmpTeamGraphLib::vertices.push_back ({{1, 0, 0}, color, normal});
    dblCmpTeamGraphLib::vertices.push_back ({{0, 1, 0}, color, normal});
    dblCmpTeamGraphLib::vertices.push_back ({{0, 0, 1}, color, normal});

    for (size_t i = 0; i < 3 * 1; ++i)
        dblCmpTeamGraphLib::indices.push_back (i);

    dblCmpTeamGraphLib::HelloTriangleApplication app;

    
    app.run ();


    return EXIT_SUCCESS;

}
