module;
// Standard Library
#include <unordered_map>
#include <vector>
export module square:char_mesh;
import :mesh;
import :circle_mesh;
import :cube_mesh;
import :cylinder_mesh;
import :line_mesh;
import :square_mesh;
import squint;

export namespace square {
/**
 * Creates a renderable text mesh containing a string of characters
 * contructed from lines.
 *
 * The char_mesh will create renderable text from
 * a string consisting of ASCII characters (excluding control characters
 * that are not the new line character). If a new line character is
 * encountered, the text will be arranged as expected.
 *
 * Each of the ASCII characters excluding the control characters have
 * been constructed out of lines on a 17 x 17 grid. The ASCII_font is
 * a collection of the indicies defining each line for each character
 * Each line is made from 4 indices (X1,Y1--X2,Y2). Each letter fits
 * within a 1x1 square in the bottom right quadrant of the screen space.
 * Default settings are applied to change the scale and position of the
 * text from this initial model space.
 *
 * There is a buffer of 2 indices on all edges of the letters to include
 * spacing of lines and letters. On occasion this buffer may be broken for
 * some special case characters.
 *
 *            _0__1__2__3__4__5__6__7__8__9__10_11_12_13_14_15_16
 *        0   |__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__| (1,0)
 *        1   |__|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|__|
 *        2   |__|xx|__|__|__|__|__|__||||__|__|__|__|__|__|xx|__|
 *        3   |__|xx|__|__|__|__|__|__||||__|__|__|__|__|__|xx|__|
 *        4   |__|xx|__|__|__|__|__|//|__|\\|__|__|__|__|__|xx|__|
 *        5   |__|xx|__|__|__|__|__|//|__|\\|__|__|__|__|__|xx|__|
 *        6   |__|xx|__|__|__|__|//|__|__|__|\\|__|__|__|__|xx|__|
 *        7   |__|xx|__|__|__|__|//|__|__|__|\\|__|__|__|__|xx|__|
 *        8   |__|xx|__|__|__|//|__|__|__|__|__|\\|__|__|__|xx|__|
 *        9   |__|xx|__|__|__|//|__|__|__|__|__|\\|__|__|__|xx|__|
 *        10  |__|xx|__|__|//|==|==|==|==|==|==|==|\\|__|__|xx|__|
 *        11  |__|xx|__|__|//|__|__|__|__|__|__|__|\\|__|__|xx|__|
 *        12  |__|xx|__|//|__|__|__|__|__|__|__|__|__|\\|__|xx|__|
 *        13  |__|xx|__|//|__|__|__|__|__|__|__|__|__|\\|__|xx|__|
 *        14  |__|xx|//|__|__|__|__|__|__|__|__|__|__|__|\\|xx|__|
 *        15  |__|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|xx|__|
 *        16  |__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|
 *           (0,-1)                                          (1,-1)
 *
 * The capital letter A is draw from (2,14)--(8,2), (8,2)--(14,14), and
 * (4,10)--(12,10) so it's index list is [2,14,8,2,8,2,14,14,4,10,12,10]
 * and will be drawn in the bottom right quadrant of the screen in model
 * space.
 *
 * After the construction of the characters on the grid, the width of the
 * characters are scaled by 1/2 the make the font look prettier. Any further
 * transformations are done using the mesh's transform after creation.
 */
class char_mesh : public composite_mesh {
  public:
    // Construct a new char_mesh.
    // 13 strokes is the largest size a single character takes up in
    // memory. This is the "@" character.
    //
    // str The string to initialize the Mesh with.
    //
    // stroke_thickness The thickness of the text in model space. If this value is
    // zero, lines will be drawn that take up a single pixel in width. The maximum
    // thickness that should be used to maintain readability is 2/17. The default
    // is zero.
    //
    // max_strokes The maximum strokes to allocate for the buffer. If zero,
    // the buffer will be initialized with enough space for the string used to
    // initialize the Mesh with, this will also disallow updating the text of the
    // Mesh. The default is zero and the text will not be able to change.
    //
    char_mesh(std::string str, float stroke_thickness = 0.0f, float extrusion_depth = 0.0f,
              unsigned int max_strokes = 0)
        : composite_mesh() {
        // Set defaults
        text = str;
        thickness = stroke_thickness;
        extrusion = extrusion_depth;
        if (max_strokes == 0) {
            is_static = true;
            max_strokes = count_strokes(str);
        }
        this->max_strokes = max_strokes;
        // generate meshes
        if (extrusion == 0.0f) {
            get_meshes().push_back(
                std::make_unique<instanced_mesh>(std::make_unique<circle_mesh>(16, 0.5f * thickness), 2 * max_strokes));
            if (thickness == 0) {
                get_meshes().push_back(std::make_unique<instanced_mesh>(std::make_unique<line_mesh>(), max_strokes));
            } else {
                get_meshes().push_back(
                    std::make_unique<instanced_mesh>(std::make_unique<line_mesh>(thickness), max_strokes));
            }

        } else {
            get_meshes().push_back(std::make_unique<instanced_mesh>(
                std::make_unique<cylinder_mesh>(0.0f, 2.0f * (float)M_PI, 16), 2 * max_strokes));
            get_meshes().push_back(std::make_unique<instanced_mesh>(std::make_unique<cube_mesh>(1.0f), max_strokes));
        }
        nodes = dynamic_cast<instanced_mesh *>(get_meshes()[0].get());
        links = dynamic_cast<instanced_mesh *>(get_meshes()[1].get());

        if (push_instances(str)) {
            return;
        }
        text = "";
    }
    squint::fvec2 get_center() const {
        squint::fvec2 label_center = {max_width / 4.0f, lines / 2.0f};
        squint::fvec3 label_scale = get_scale();
        squint::fvec2 true_label_center = {
            get_position()[0].data()[0].as_meters() + 2.0f * label_scale[0] * label_center[0],
            get_position()[1].data()[0].as_meters() - 2.0f * label_scale[1] * label_center[1]};
        return true_label_center;
    }
    void set_center(squint::fvec2 center) {
        squint::fvec2 label_center = {max_width / 4.0f, lines / 2.0f};
        squint::fvec3 label_scale = get_scale();
        squint::fvec2 label_top_left =
            squint::fvec2({center[0] - label_scale[0] * label_center[0], center[1] + label_scale[1] * label_center[1]});
        squint::fvec3 position({label_top_left[0].data()[0], label_top_left[1].data()[0], 0.0f});
        set_position(position.view_as<squint::quantities::length_f>());
    }
    void set_text(std::string str) {
        if (!is_static) {
            std::string old_string = text;
            text = str;
            lines = 0;
            max_width = 0;
            stroke_count = 0;
            nodes->clear_instances();
            links->clear_instances();
            if (push_instances(str)) {
                return;
            }
            push_instances(old_string);
            text = old_string;
        }
    }
    std::string get_text() { return text; }

  private:
    int lines = 0;
    unsigned int max_width = 0;
    std::string text;
    float thickness;
    float extrusion;
    unsigned int stroke_count = 0;
    unsigned int max_strokes;
    bool is_static = false;
    instanced_mesh *nodes;
    instanced_mesh *links;
    float grid_points[17] = {0.0f,    0.0625f, 0.125f,  0.1875f, 0.25f,   0.3125f, 0.375f,  0.4375f, 0.5f,
                             0.5625f, 0.625f,  0.6875f, 0.75f,   0.8125f, 0.875f,  0.9375f, 1.0f};
    std::unordered_map<char, std::vector<unsigned int>> ASCII_font = {
        {'!', std::vector<unsigned int>{8, 2, 8, 11, 8, 13, 8, 14}},
        {'"', std::vector<unsigned int>{5, 2, 5, 4, 11, 2, 11, 4}},
        {'#', std::vector<unsigned int>{7, 2, 3, 14, 13, 2, 9, 14, 2, 5, 14, 5, 2, 11, 14, 11}},
        {'$',
         std::vector<unsigned int>{2, 12, 14, 12, 14, 12, 14, 8, 14, 8, 2, 8, 2, 8, 2, 4, 2, 4, 14, 4, 8, 2, 8, 14}},
        {'%', std::vector<unsigned int>{2,  2, 2,  6,  2,  6,  6,  6,  6,  6,  6,  2,  6,  2,  2,  2,  2,  14,
                                        14, 2, 10, 10, 10, 14, 10, 14, 14, 14, 14, 14, 14, 10, 14, 10, 10, 10}},
        {'&',
         std::vector<unsigned int>{12, 14, 6,  6, 6, 6,  6, 4,  6, 4,  7, 2,  7, 2,  9, 2,  9,  2,  10, 4,  10, 4,
                                   10, 6,  10, 6, 4, 10, 4, 10, 4, 13, 4, 13, 6, 14, 6, 14, 10, 14, 10, 14, 12, 11}},
        {'\'', std::vector<unsigned int>{8, 2, 8, 4}},
        {'(', std::vector<unsigned int>{8, 2, 6, 6, 6, 6, 6, 10, 6, 10, 8, 14}},
        {')', std::vector<unsigned int>{8, 2, 10, 6, 10, 6, 10, 10, 10, 10, 8, 14}},
        {'*', std::vector<unsigned int>{8, 5, 8, 11, 6, 6, 10, 10, 6, 10, 10, 6}},
        {'+', std::vector<unsigned int>{8, 5, 8, 11, 5, 8, 11, 8}},
        {',', std::vector<unsigned int>{7, 14, 9, 14, 9, 14, 9, 12}},
        {'-', std::vector<unsigned int>{5, 8, 11, 8}},
        {'.', std::vector<unsigned int>{8, 13, 8, 14}},
        {'/', std::vector<unsigned int>{11, 2, 5, 14}},
        {'0', std::vector<unsigned int>{2, 2, 14, 2, 14, 2, 14, 14, 14, 14, 2, 14, 2, 14, 2, 2, 2, 14, 14, 2}},
        {'1', std::vector<unsigned int>{6, 4, 8, 2, 8, 2, 8, 14, 6, 14, 10, 14}},
        {'2',
         std::vector<unsigned int>{2, 4, 2, 2, 2, 2, 14, 2, 14, 2, 14, 8, 14, 8, 2, 8, 2, 8, 2, 14, 2, 14, 14, 14}},
        {'3', std::vector<unsigned int>{2, 2, 14, 2, 2, 8, 14, 8, 2, 14, 14, 14, 14, 2, 14, 14}},
        {'4', std::vector<unsigned int>{2, 2, 2, 8, 14, 2, 14, 14, 2, 8, 14, 8}},
        {'5',
         std::vector<unsigned int>{14, 2, 2, 2, 2, 2, 2, 8, 2, 8, 14, 8, 14, 8, 14, 14, 14, 14, 2, 14, 2, 14, 2, 12}},
        {'6',
         std::vector<unsigned int>{14, 4, 14, 2, 14, 2, 2, 2, 2, 2, 2, 14, 2, 14, 14, 14, 14, 14, 14, 8, 14, 8, 2, 8}},
        {'7', std::vector<unsigned int>{2, 2, 14, 2, 14, 2, 2, 14}},
        {'8', std::vector<unsigned int>{2, 2, 14, 2, 14, 2, 14, 14, 14, 14, 2, 14, 2, 14, 2, 2, 2, 8, 14, 8}},
        {'9',
         std::vector<unsigned int>{14, 8, 2, 8, 2, 8, 2, 2, 2, 2, 14, 2, 14, 2, 14, 14, 14, 14, 2, 14, 2, 14, 2, 12}},
        {':', std::vector<unsigned int>{8, 3, 8, 5, 8, 13, 8, 11}},
        {';', std::vector<unsigned int>{8, 3, 8, 5, 7, 14, 9, 14, 9, 14, 9, 12}},
        {'<', std::vector<unsigned int>{14, 2, 2, 8, 2, 8, 14, 14}},
        {'=', std::vector<unsigned int>{2, 5, 14, 5, 2, 11, 14, 11}},
        {'>', std::vector<unsigned int>{2, 2, 14, 8, 14, 8, 2, 14}},
        {'?', std::vector<unsigned int>{2, 4, 2, 2, 2, 2, 14, 2, 14, 2, 14, 8, 14, 8, 8, 8, 8, 8, 8, 11, 8, 13, 8, 14}},
        {'@', std::vector<unsigned int>{10, 10, 6,  10, 6,  10, 6,  6,  6,  6,  10, 6,  10, 6,  10, 12, 10, 12,
                                        12, 12, 12, 12, 14, 10, 14, 10, 14, 6,  14, 6,  10, 2,  10, 2,  6,  2,
                                        6,  2,  2,  6,  2,  6,  2,  10, 2,  10, 6,  14, 6,  14, 10, 14}},
        {'A', std::vector<unsigned int>{2, 14, 8, 2, 8, 2, 14, 14, 4, 10, 12, 10}},
        {'B',
         std::vector<unsigned int>{2, 2, 2, 14, 2, 2, 11, 2, 11, 2, 11, 7, 2, 7, 14, 7, 2, 14, 14, 14, 14, 14, 14, 7}},
        {'C', std::vector<unsigned int>{14, 4, 14, 2, 14, 2, 2, 2, 2, 2, 2, 14, 2, 14, 14, 14, 14, 14, 14, 12}},
        {'D',
         std::vector<unsigned int>{2, 2, 8, 2, 8, 2, 14, 5, 14, 5, 14, 11, 14, 11, 8, 14, 8, 14, 2, 14, 2, 14, 2, 2}},
        {'E', std::vector<unsigned int>{2, 14, 2, 2, 2, 2, 14, 2, 2, 8, 8, 8, 2, 14, 14, 14}},
        {'F', std::vector<unsigned int>{2, 14, 2, 2, 2, 2, 14, 2, 2, 8, 8, 8}},
        {'G',
         std::vector<unsigned int>{12, 4, 12, 2, 12, 2, 2, 2, 2, 2, 2, 14, 2, 14, 12, 14, 12, 14, 12, 8, 14, 8, 8, 8}},
        {'H', std::vector<unsigned int>{2, 2, 2, 14, 14, 2, 14, 14, 2, 8, 14, 8}},
        {'I', std::vector<unsigned int>{2, 2, 14, 2, 2, 14, 14, 14, 8, 2, 8, 14}},
        {'J', std::vector<unsigned int>{2, 2, 14, 2, 11, 2, 11, 11, 11, 11, 8, 14, 8, 14, 5, 14, 5, 14, 2, 11}},
        {'K', std::vector<unsigned int>{2, 2, 2, 14, 2, 8, 14, 2, 2, 8, 14, 14}},
        {'L', std::vector<unsigned int>{2, 2, 2, 14, 2, 14, 14, 14}},
        {'M', std::vector<unsigned int>{2, 14, 2, 2, 2, 2, 8, 11, 8, 11, 14, 2, 14, 2, 14, 14}},
        {'N', std::vector<unsigned int>{2, 14, 2, 2, 2, 2, 14, 14, 14, 14, 14, 2}},
        {'O', std::vector<unsigned int>{2, 2, 14, 2, 14, 2, 14, 14, 14, 14, 2, 14, 2, 14, 2, 2}},
        {'P', std::vector<unsigned int>{2, 14, 2, 2, 2, 2, 14, 2, 14, 2, 14, 8, 14, 8, 2, 8}},
        {'Q', std::vector<unsigned int>{2, 2, 14, 2, 14, 2, 14, 14, 14, 14, 2, 14, 2, 14, 2, 2, 11, 11, 16, 16}},
        {'R', std::vector<unsigned int>{2, 14, 2, 2, 2, 2, 14, 2, 14, 2, 14, 8, 14, 8, 2, 8, 8, 8, 14, 14}},
        {'S', std::vector<unsigned int>{14, 5, 14, 2, 14, 2,  2,  2,  2, 2,  2, 8,  2, 8,
                                        14, 8, 14, 8, 14, 14, 14, 14, 2, 14, 2, 14, 2, 11}},
        {'T', std::vector<unsigned int>{2, 2, 14, 2, 8, 2, 8, 14}},
        {'U', std::vector<unsigned int>{2, 2, 2, 14, 2, 14, 14, 14, 14, 14, 14, 2}},
        {'V', std::vector<unsigned int>{2, 2, 8, 14, 8, 14, 14, 2}},
        {'W', std::vector<unsigned int>{2, 2, 2, 14, 2, 14, 8, 5, 8, 5, 14, 14, 14, 14, 14, 2}},
        {'X', std::vector<unsigned int>{2, 2, 14, 14, 14, 2, 2, 14}},
        {'Y', std::vector<unsigned int>{2, 2, 8, 8, 8, 8, 14, 2, 8, 8, 8, 14}},
        {'Z', std::vector<unsigned int>{2, 2, 14, 2, 14, 2, 2, 14, 2, 14, 14, 14}},
        {'[', std::vector<unsigned int>{8, 0, 2, 0, 2, 0, 2, 16, 2, 16, 8, 16}},
        {'\\', std::vector<unsigned int>{5, 2, 11, 14}},
        {']', std::vector<unsigned int>{8, 0, 14, 0, 14, 0, 14, 16, 14, 16, 8, 16}},
        {'^', std::vector<unsigned int>{5, 5, 8, 2, 8, 2, 11, 5}},
        {'_', std::vector<unsigned int>{0, 16, 16, 16}},
        {'`', std::vector<unsigned int>{7, 2, 9, 5}},
        {'a', std::vector<unsigned int>{2, 10, 2, 14, 2, 14, 14, 14, 14, 14, 14, 8, 14, 10, 2, 10, 14, 8, 2, 8}},
        {'b', std::vector<unsigned int>{2, 2, 2, 14, 2, 14, 14, 14, 14, 14, 14, 8, 14, 8, 2, 8}},
        {'c', std::vector<unsigned int>{2, 8, 2, 14, 2, 14, 14, 14, 14, 8, 2, 8}},
        {'d', std::vector<unsigned int>{2, 8, 2, 14, 2, 14, 14, 14, 14, 14, 14, 2, 14, 8, 2, 8}},
        {'e', std::vector<unsigned int>{2, 8, 2, 14, 2, 14, 14, 14, 14, 11, 14, 8, 14, 8, 2, 8, 2, 11, 14, 11}},
        {'f', std::vector<unsigned int>{8, 14, 8, 2, 8, 2, 14, 2, 14, 2, 14, 5, 2, 8, 14, 8}},
        {'g', std::vector<unsigned int>{2, 8, 2, 14, 2, 14, 14, 14, 14, 16, 14, 8, 14, 8, 2, 8, 14, 16, 2, 16}},
        {'h', std::vector<unsigned int>{2, 14, 2, 2, 2, 8, 14, 8, 14, 8, 14, 14}},
        {'i', std::vector<unsigned int>{8, 5, 8, 14, 8, 2, 8, 3}},
        {'j', std::vector<unsigned int>{8, 5, 8, 16, 8, 16, 2, 16, 8, 2, 8, 3}},
        {'k', std::vector<unsigned int>{2, 2, 2, 14, 2, 11, 14, 8, 2, 11, 14, 14}},
        {'l', std::vector<unsigned int>{8, 2, 8, 14, 8, 14, 10, 14, 8, 2, 6, 2}},
        {'m', std::vector<unsigned int>{2, 8, 2, 14, 8, 8, 8, 14, 14, 8, 14, 14, 2, 8, 14, 8}},
        {'n', std::vector<unsigned int>{2, 8, 2, 14, 14, 8, 14, 14, 2, 8, 14, 8}},
        {'o', std::vector<unsigned int>{2, 8, 2, 14, 14, 8, 14, 14, 2, 8, 14, 8, 2, 14, 14, 14}},
        {'p', std::vector<unsigned int>{2, 8, 2, 16, 14, 8, 14, 14, 2, 8, 14, 8, 2, 14, 14, 14}},
        {'q', std::vector<unsigned int>{2, 8, 2, 14, 14, 8, 14, 16, 2, 8, 14, 8, 2, 14, 14, 14}},
        {'r', std::vector<unsigned int>{2, 8, 2, 14, 14, 8, 14, 11, 2, 8, 14, 8}},
        {'s', std::vector<unsigned int>{2, 8, 2, 11, 14, 11, 14, 14, 2, 8, 14, 8, 2, 14, 14, 14, 2, 11, 14, 11}},
        {'t', std::vector<unsigned int>{8, 2, 8, 14, 2, 5, 14, 5, 8, 14, 14, 14}},
        {'u', std::vector<unsigned int>{2, 8, 2, 14, 14, 8, 14, 14, 2, 14, 14, 14}},
        {'v', std::vector<unsigned int>{2, 8, 8, 14, 8, 14, 14, 8}},
        {'w', std::vector<unsigned int>{2, 8, 5, 14, 5, 14, 8, 8, 8, 8, 11, 14, 11, 14, 14, 8}},
        {'x', std::vector<unsigned int>{2, 8, 14, 14, 14, 8, 2, 14}},
        {'y', std::vector<unsigned int>{2, 8, 8, 14, 6, 16, 14, 8}},
        {'z', std::vector<unsigned int>{2, 8, 14, 8, 14, 8, 2, 14, 2, 14, 14, 14}},
        {'{', std::vector<unsigned int>{10, 2, 8, 2, 8, 2, 7, 4,  7, 4,  7, 7,  7, 7,  5,  8,
                                        5,  8, 7, 9, 7, 9, 7, 12, 7, 12, 8, 14, 8, 14, 10, 14}},
        {'|', std::vector<unsigned int>{8, 0, 8, 16}},
        {'}', std::vector<unsigned int>{6,  2, 8, 2, 8, 2, 9, 4,  9, 4,  9, 7,  9, 7,  11, 8,
                                        11, 8, 9, 9, 9, 9, 9, 12, 9, 12, 8, 14, 8, 14, 6,  14}},
        {'~', std::vector<unsigned int>{2,  10, 2,  8,  2,  8,  4,  6,  4,  6, 6,  6, 6,  6,
                                        10, 10, 10, 10, 12, 10, 12, 10, 14, 8, 14, 8, 14, 6}}};
    unsigned int count_strokes(std::string str) {
        std::vector<unsigned int> indices{};
        for (auto &c : str) {
            for (auto i : ASCII_font[c]) {
                indices.push_back(i);
            }
        }
        return indices.size() / 4;
    }
    void push_char(unsigned int column, unsigned int row, char letter) {
        std::vector<squint::fmat4> transforms{};
        squint::fvec3 offset = squint::fvec3({0.5f * static_cast<float>(column), -static_cast<float>(row), 0.0f});
        std::vector<unsigned int> indices = ASCII_font[letter];
        for (size_t i = 0; i < indices.size() / 4; i++) {

            squint::fvec3 p1 = {0.5f * grid_points[indices[4 * i]], -grid_points[indices[4 * i + 1]], 0.0f};
            squint::fvec3 p2 = {0.5f * grid_points[indices[4 * i + 2]], -grid_points[indices[4 * i + 3]], 0.0f};
            p1 += offset;
            p2 += offset;
            squint::fvec3 p3 = 0.5f * (p1 + p2);
            // push node instances
            if (thickness != 0.0f && extrusion == 0.0f) {
                nodes->push_instance(transform(squint::translate(squint::fmat4::I(), p1)));
                nodes->push_instance(transform(squint::translate(squint::fmat4::I(), p2)));
            } else {
                squint::fmat4 node1_instance =
                    squint::translate(squint::fmat4::I(), p1 - squint::fvec3({0.0f, 0.0f, 0.5f * extrusion})) *
                    squint::scale(squint::fmat4::I(), squint::fvec3({thickness, thickness, extrusion}));
                squint::fmat4 node2_instance =
                    squint::translate(squint::fmat4::I(), p2 - squint::fvec3({0.0f, 0.0f, 0.5f * extrusion})) *
                    squint::scale(squint::fmat4::I(), squint::fvec3({thickness, thickness, extrusion}));

                nodes->push_instance(transform(node1_instance));
                nodes->push_instance(transform(node2_instance));
            }
            // push link instance
            float angle = atan2f(p2[1] - p1[1], p2[0] - p1[0]);
            float length = squint::norm(p2 - p1);
            if (extrusion == 0.0f) {
                squint::fmat4 link_instance =
                    squint::translate(squint::fmat4::I(), p3) *
                    squint::rotate(squint::fmat4::I(), angle, squint::fvec3({0.0f, 0.0f, 1.0f})) *
                    squint::scale(squint::fmat4::I(), squint::fvec3({length, 1.0f, 1.0f}));
                links->push_instance(transform(link_instance));
            } else {
                squint::fmat4 link_instance =
                    squint::translate(squint::fmat4::I(), p3) *
                    squint::rotate(squint::fmat4::I(), angle, squint::fvec3({0.0f, 0.0f, 1.0f})) *
                    squint::scale(squint::fmat4::I(), squint::fvec3({length, thickness, extrusion}));
                links->push_instance(transform(link_instance));
            }

            stroke_count++;
        }
    }
    bool push_instances(std::string str) {
        if (max_strokes >= count_strokes(str)) {
            unsigned int column = 0;
            unsigned int row = 0;
            for (auto &c : str) {
                if (c == '\n') {
                    column = 0;
                    row++;
                    continue;
                } else {
                    push_char(column, row, c);
                    if (column + 1 > max_width) {
                        max_width = column + 1;
                    }
                    column++;
                }
            }
            lines = row + 1;
            return true;
        }
        return false;
    }
};
} // namespace square
