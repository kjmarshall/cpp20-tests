export module BasicPlane.Figures:Rectangle; // defines the module partition Rectangle

// import makes all the exported functions, types, etc... visible
import :Point;

export struct Rectangle // make this struct visible to importers
{
    Point ul, lr;
};

// These functions are declared, but will
// be defined in a module implementation file
export int area(const Rectangle &r);
export int height(const Rectangle &r);
export int width(const Rectangle &r);

// Other notes
// Omit export if you don't want to export something (seems reasonable)
// Use "export import" to make an imported module or module partition visible
// module :private makrs beginning of private module parittion where declarations
// are only visible to the file