function show_image(src, dest)
  colormap gray;
  A = readmatrix(src);
  image(A);
  if nargin > 1
    saveas(gcf, dest);
  end

end
