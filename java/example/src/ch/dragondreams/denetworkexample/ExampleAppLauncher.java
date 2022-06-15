package ch.dragondreams.denetworkexample;

import com.jdotsoft.jarloader.JarClassLoader;

public class ExampleAppLauncher {
	public static void main(String[] args) {
		JarClassLoader jcl = new JarClassLoader();
		try {
			jcl.invokeMain("ch.dragondreams.denetworkexample.ExampleApp", args);
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}
}
