function show_image(filename, imagename)
  colormap gray;
  A = readmatrix(filename);
  image(A);

end
