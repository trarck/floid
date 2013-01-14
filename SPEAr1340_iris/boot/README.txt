The run.img file is made using the mkimage command:
>mkimage -n "Iris Platform uBoot Run Script" -A arm  -T script -C none  -d run.txt run.img

It’s possible to remove the header information with the dd command:
>dd if=run.img of=run.txt bs=72 skip=1

Create an ad-hoc raw image with an image manipulation program like Gimp, see below.

1.Start up Gimp
2.Create a new image with the desired resolution (e.g. 800x480)
3.Add an empty alpha channel: Layer->Transparency->Add Alpha Channel
4.Modify the image to its final aspect
5.Save the image (for future reference) in e.g. png format
6.Now we need to re-arrange the color channels to match the physical layout in framebuffer memory
  a.Colors->Components->Decompose…
  b.Select RGBA color model and Decompose to layers and click OK: this will create a new image
  c.Re-compose manually the color channels in the new image with Colors->Components->Compose…
  d.Select RGBA color model
  e.Change the channel order from Red-Green-Blue-Alpha to Blue-Green-Red-Alpha (e.g. swap the red and blue layers)
  f.Click OK, it will appear an image with mixed-up colors, export it to raw format (Raw Image data) with a suitable filename (800x480x32.raw)
  g.Select Standard (R,G,B) and click on Export
  h.The raw image data is now ready

