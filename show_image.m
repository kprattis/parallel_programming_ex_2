function show_image(src, dest)
  colormap gray;
  A = readmatrix(src);
  image(A);
  set(gca,'visible','off');
  if nargin > 1
    saveas(gcf, dest);
  end

end