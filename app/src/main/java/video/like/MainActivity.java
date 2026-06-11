package video.like;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.res.AssetManager;
import android.graphics.Color;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.view.View;
import android.widget.TextView;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.List;
import android.graphics.drawable.AnimationDrawable;
import android.widget.LinearLayout;
import android.animation.ArgbEvaluator;
import android.animation.ValueAnimator;
import android.graphics.drawable.GradientDrawable;
import android.widget.Button;
import android.widget.Toast;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.net.InetSocketAddress;
import java.net.Socket;

public class MainActivity extends Activity {

    static String packageGame;
    String url;
    String libPath;
    String ownLibPath;
    String architecture;
    boolean wasPermissionDenied = false; 

	@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        System.loadLibrary("User");	

		try {
			AssetManager assetManager = getAssets();
			String[] assetsList = getAllExecuterFiles();
			for (String execudeFiles : assetsList) {
				InputStream inputStream = assetManager.open(execudeFiles);              
				String outputPath = getFilesDir().getPath() + File.separator + execudeFiles;
				OutputStream outputStream = new FileOutputStream(outputPath);

				byte[] mamkoByte = new byte[1024];
				int sex;
				while ((sex = inputStream.read(mamkoByte)) > 0) {
					outputStream.write(mamkoByte, 0, sex);
				}
				outputStream.close();
				inputStream.close();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}

		procDestroy();
		url = getUrl();
		packageGame = getPackageGame(); 
		String file1 = "/data/data/" + packageGame + "/files/Shaders.txt";
        String file2 = "/data/data/" + packageGame + "/files/log.txt";
		deleteFiles(file1, file2);
        getOwnLibraryPath();
		copyLibToTmp();
        checkPackage(packageGame);
        checkRoot();
        checkArchitecture();
        checkPermission();
        checkFloating(this);
		architecture = getArchitecture();      
        extractArm64LibForEmulator();
		startSocketCheck();
    }

    @Override
    protected void onResume() {
        super.onResume();     
        checkPermission(); 
    }

    private void checkPermission() {
        TextView floatCheckTv = findViewById(R.id.floatCheckTv);
        if (floatCheckTv == null) return;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (!Settings.canDrawOverlays(this)) {          
                floatCheckTv.setText("Floating permission not granted");
                floatCheckTv.setTextColor(Color.RED);
                wasPermissionDenied = true;
            } else {
                
                if (wasPermissionDenied) {
                    floatCheckTv.setText("Need reboot app");
                    floatCheckTv.setTextColor(Color.YELLOW);
                } else {                 
                    floatCheckTv.setText("");
                }
            }
        }
    }

    native void checkFloating(Context ctx);

    native void nativePath(String libPath);

    native void nativeOwnPath(String ownLibPath);

	static native void procDestroy();

	static native String getPackageGame();

    native String getUrl();

    public void getOwnLibraryPath() {
        try {
            ApplicationInfo appInfo = getApplicationInfo();
            ownLibPath = appInfo.nativeLibraryDir;
        } catch (Exception e) {
            e.printStackTrace();
        }
        nativeOwnPath(ownLibPath);
    }

    public void openGameBtn(View v) {
        PackageManager paackGame = getPackageManager();
        Intent mainIntent = new Intent(Intent.ACTION_MAIN, null);
        mainIntent.addCategory(Intent.CATEGORY_LAUNCHER);
        List<ResolveInfo> apps = paackGame.queryIntentActivities(mainIntent, 0);
        for (ResolveInfo app : apps) {
            if (app.activityInfo.packageName.equals(packageGame)) {
                Intent intent = new Intent(Intent.ACTION_MAIN);
                intent.setClassName(app.activityInfo.packageName, app.activityInfo.name);
                startActivity(intent);
                break;
            }
        }
    }

    public void openLinkBtn(View v) {
        Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.setData(Uri.parse(url));
        startActivity(intent);
    }

	public void openSettingsBtn(View v) {
		Intent intent = new Intent(this, SettingsActivity.class);
		startActivity(intent);
	}

    private void checkPackage(String packageName) {
        TextView packageCheckTextView = findViewById(R.id.PackageCheckTv);
        PackageManager pm = getPackageManager();

        try {
            PackageInfo packageInfo = pm.getPackageInfo(packageName, PackageManager.GET_ACTIVITIES);
            String gameName = packageInfo.applicationInfo.loadLabel(pm).toString();
            String version = packageInfo.versionName;
            packageCheckTextView.setText("Game: " + gameName + " " + version);
            packageCheckTextView.setTextColor(Color.parseColor("#FFFFFFFF"));           
        } catch (PackageManager.NameNotFoundException e) {
            packageCheckTextView.setText("Game: not found");
            packageCheckTextView.setTextColor(Color.parseColor("#FF0000"));
        }
    }

    private String getArchitecture() {
        return Build.SUPPORTED_ABIS[0];
    }

    void checkArchitecture() {
        TextView architectureTextView = findViewById(R.id.ArchitectureCheckTv);
        String arch = getArchitecture();

        String displayText;
        int textColor;
        if ("arm64-v8a".equals(arch) || "x86_64".equals(arch)) {
            displayText = "Architecture: " + arch;
            textColor = Color.WHITE; 
        } else {
            displayText = "Architecture: " + arch + " (unsupported!)";
            textColor = Color.RED;
        }
        architectureTextView.setText(displayText);
        architectureTextView.setTextColor(textColor);
    }

    private void startSocketCheck() {
		final TextView socketTextView = findViewById(R.id.SocketCheckTv);
		socketTextView.setText("Wait socket connection...");
		socketTextView.setTextColor(Color.YELLOW);

		new Thread(new Runnable() {
				@Override
				public void run() {
					long startTime = System.currentTimeMillis();
					boolean isConnected = false;				
					while (System.currentTimeMillis() - startTime < 8000) {
						Socket socket = null;
						try {
							socket = new Socket();					
							socket.connect(new InetSocketAddress("127.0.0.1", 5588), 400);
							isConnected = true; 
							break;
						} catch (IOException e) {						
							try {
								Thread.sleep(200);
							} catch (InterruptedException ie) {
								Thread.currentThread().interrupt();
								break;
							}
						} finally {
							if (socket != null) {
								try {
									socket.close();
								} catch (IOException e) {
									e.printStackTrace();
								}
							}
						}
					}

					final boolean success = isConnected;
					runOnUiThread(new Runnable() {
							@Override
							public void run() {
								if (success) {
									socketTextView.setText("Success socket connection");
									socketTextView.setTextColor(Color.GREEN);
								} else {
									socketTextView.setText("Failed socket connection");
									socketTextView.setTextColor(Color.RED);
								}
							}
						});
				}
			}).start();
	}

	private boolean copyLibToTmp() {
		ZipFile zipFile = null;
		Process process = null;
		DataOutputStream os = null;
		try {	
			ApplicationInfo appInfo = getApplicationInfo();
			String apkPath = appInfo.sourceDir; 
			zipFile = new ZipFile(apkPath);
			File cacheDir = getCacheDir();		
			File tempA64 = new File(cacheDir, "temp_a64.so");
			File tempX64 = new File(cacheDir, "temp_x64.so");	
			boolean hasA64 = extractFileFromApk(zipFile, "lib/arm64-v8a/libMemory.so", tempA64);
			boolean hasX64 = extractFileFromApk(zipFile, "lib/x86_64/libMemory.so", tempX64);

			process = Runtime.getRuntime().exec("su");
			os = new DataOutputStream(process.getOutputStream());

			if (hasA64) {
				String destA64 = "/data/local/tmp/libMemoryA64.so";
				os.writeBytes("cp " + tempA64.getAbsolutePath() + " " + destA64 + "\n");
				os.writeBytes("chmod 755 " + destA64 + "\n");			
			}

			if (hasX64) {
				String destX64 = "/data/local/tmp/libMemoryX64.so";
				os.writeBytes("cp " + tempX64.getAbsolutePath() + " " + destX64 + "\n");
				os.writeBytes("chmod 755 " + destX64 + "\n");		
			}

			os.writeBytes("exit\n");
			os.flush();
			int exitValue = process.waitFor();
			if (tempA64.exists()) tempA64.delete();
			if (tempX64.exists()) tempX64.delete();			
			return true;

		} catch (IOException | InterruptedException e) {
			e.printStackTrace();
			return false;
		} finally {
			if (zipFile != null) {
				try { zipFile.close(); } catch (IOException ignored) {}
			}
			if (os != null) {
				try { os.close(); } catch (IOException ignored) {}
			}
			if (process != null) {
				process.destroy();
			}
		}
	}

	private boolean extractFileFromApk(ZipFile zipFile, String entryPath, File destFile) {
		ZipEntry entry = zipFile.getEntry(entryPath);
		InputStream is = null;
		OutputStream os = null;
		try {
			is = zipFile.getInputStream(entry);
			os = new FileOutputStream(destFile);

			byte[] buffer = new byte[4096];
			int bytesRead;
			while ((bytesRead = is.read(buffer)) != -1) {
				os.write(buffer, 0, bytesRead);
			}
			return true;
		} catch (IOException e) {	
			e.printStackTrace();
			return false;
		} finally {		
			if (is != null) {
				try {
					is.close();
				} catch (IOException ignored) {}
			}
			if (os != null) {
				try {
					os.close();
				} catch (IOException ignored) {}
			}
		}
	}

    private boolean copyMemoryLib(InputStream inputStream, String destinationPath) {
        File tempFile = null;
        try {
            tempFile = File.createTempFile("libMemory_temp", ".so", getCacheDir());
            try (OutputStream tempOut = new FileOutputStream(tempFile)) {
                byte[] buffer = new byte[4096];
                int bytesRead;
                while ((bytesRead = inputStream.read(buffer)) != -1) {
                    tempOut.write(buffer, 0, bytesRead);
                }
            }

            Process process = Runtime.getRuntime().exec("su");
            try (DataOutputStream os = new DataOutputStream(process.getOutputStream())) {
                os.writeBytes("cp " + tempFile.getAbsolutePath() + " " + destinationPath + "\n");
                os.writeBytes("chmod 755 " + destinationPath + "\n");
                os.writeBytes("exit\n");
                os.flush();
            }

            int exitValue = process.waitFor();
            return exitValue == 0;
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
            return false;
        } finally {
            if (tempFile != null && tempFile.exists()) {
                tempFile.delete();
            }
        }
    }

    private void extractArm64LibForEmulator() {
        if (!"x86_64".equals(getArchitecture())) {
            return;
        }

        if (ownLibPath == null || ownLibPath.isEmpty()) {
            getOwnLibraryPath();
        }

        ZipFile zipFile = null;
        InputStream inputStream = null;
        try {
            ApplicationInfo appInfo = getApplicationInfo();
            String apkPath = appInfo.sourceDir;

            zipFile = new ZipFile(apkPath);       
            String libPathInApk = "lib" + File.separator + "arm64-v8a" + File.separator + "libmain.so";
            ZipEntry libEntry = zipFile.getEntry(libPathInApk);

            if (libEntry != null) {
                inputStream = zipFile.getInputStream(libEntry);
                String destinationPath = ownLibPath + File.separator + "libmain.so";
                if (copyMemoryLib(inputStream, destinationPath)) {                
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (inputStream != null) inputStream.close();
                if (zipFile != null) zipFile.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private String[] getAllExecuterFiles() {
		return new String[]{
			"executer", 
			"injectorLib", 
			"executerX64", 
			"injectorLibX64"
		};
	}

    void checkRoot() {
        TextView rootTextView = findViewById(R.id.RootCheckTv);
        Process process = null;
        DataOutputStream os = null;
        try {
            process = Runtime.getRuntime().exec("su");
            os = new DataOutputStream(process.getOutputStream());
            os.writeBytes("touch /data/testfile.txt\n");
            os.writeBytes("exit\n");
            os.flush();

            int exitValue = process.waitFor();
            if (exitValue == 0) {
                rootTextView.setText("Root: found");
                rootTextView.setTextColor(Color.parseColor("#FFFFFFFF"));
            } else {
                rootTextView.setText("Root: not found (Cheat dont work without root!)");
                rootTextView.setTextColor(Color.parseColor("#FF0000"));
            }
        } catch (Exception e) {
            rootTextView.setText("Root: check error");
            rootTextView.setTextColor(Color.parseColor("#FF0000"));
        } finally {
            try {
                if (os != null) {
                    os.close();
                }
                if (process != null) {
                    process.destroy();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        File testFile = new File("/data/testfile.txt"); 
        if (testFile.exists()) {
            testFile.delete();
        }
    }

	public boolean deleteFiles(String... filePaths) {
        try {
            Process process = Runtime.getRuntime().exec("su");
            DataOutputStream os = new DataOutputStream(process.getOutputStream());
            for (String filePath : filePaths) {
                os.writeBytes("rm -f \"" + filePath + "\"\n");
            }
            os.writeBytes("exit\n");
            os.flush();
            process.waitFor();
            os.close();
            return true;
        } catch (IOException | InterruptedException e) {
            return false;
        }
    }

	@Override
    public void onBackPressed() {
        Intent intent = new Intent(this, Ui.class);
        stopService(intent);
        System.exit(0);	
    }

    public void onDestroy() {
        super.onDestroy();
        Intent intent = new Intent(this, Ui.class);
        stopService(intent);
	    System.exit(0); 
    }
}
