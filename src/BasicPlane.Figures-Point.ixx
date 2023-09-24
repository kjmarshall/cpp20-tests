// Note that the : defines a module partition so this can only be used in
// files that are part of the named module BasicPlane.Figures
export module BasicPlane.Figures:Point; // defines a module partition, Point, that's part of the module BasicPlane.Figures

// export makes Point visible to consumers of the module
export struct Point {
    int x, y;
};