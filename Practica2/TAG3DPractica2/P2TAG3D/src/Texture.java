import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

/**
 * Clase que carga las im‡genes de las texturas en memoria
 * CLASE CREADA POR NOSOTROS
 *
 *
 */
public class Texture {
	/**
	 * Nombre del archivo de la textura
	 */
	public String file = null;
	/**
	 * Aplicar filtrado lineal
	 */
	public int filter = 1;
	
	
	/**
	 * Lee una imagen y la convierte a un formato manejable por el programa.
	 * @param fname
	 * @return
	 */
	public ColorImage loadTexture () {
		BufferedImage img = null;

		try {
			img = ImageIO.read(new File(file));
		} catch (IOException e) {
			System.out.println("Texture: "+file+" - "+e.getMessage());
			System.exit(1);
		}
		ColorImage tex = new ColorImage(img.getWidth(),img.getHeight());
		tex.filter = (this.filter == 1);
		
		for(int x = 0; x < img.getWidth(); x ++) {
			for (int y = 0; y < img.getHeight(); y++) {
				Color c = new Color();
				c.fromPackedInt(img.getRGB(x, y)); 
				tex.setColor(x, (img.getHeight()-1)-y, c);
			}
		}
		return tex;
	}
	
	/**
	 * Comprueba los par‡metros de entrada.
	 */
	public void initFromParser() {
		// Comprobar par‡metros
		if (this.file == null){
			System.err.println("Texture: Falta el archivo de la textura <file>");
			System.exit(1);
		}
	}
}
