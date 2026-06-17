package video.like;

import android.animation.ArgbEvaluator;
import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.app.AlertDialog;
import android.app.Service;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.Typeface;
import android.graphics.PixelFormat;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.text.Html;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.TextView;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import android.content.SharedPreferences;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.widget.ArrayAdapter;
import android.os.Looper;

public class Ui extends Service {
    private static Ui instance;

    public static Ui getInstance() {
        return instance;
    }

    static boolean enableBottomAnimation;
    static boolean enableAnimation;
    static boolean enableBottom;
    private SharedPreferences sharedPreferences;
	public native void StringCases(int id, String text);
	private ArrayList<String> shaderList = new ArrayList<String>();
	private ArrayAdapter<String> shaderAdapter;

    private static final int CLICK_DURATION_THRESHOLD = 200;
    private final java.util.Map<String, java.util.List<Runnable>> subWindowItems =
	new java.util.LinkedHashMap<>();
    private final java.util.Map<String, LinearLayout> subWindowContainers =
	new java.util.LinkedHashMap<>();

    private final java.util.Map<String, Button> subWindowToggles =
	new java.util.LinkedHashMap<>();
    private String currentSubWindow = null;

	int[] rainbowColors = {
		Color.rgb(255, 0, 0), Color.rgb(255, 140, 0), Color.rgb(255, 255, 0), Color.rgb(0, 255, 0),
		Color.rgb(0, 191, 255), Color.rgb(106, 90, 205), Color.rgb(199, 21, 133), Color.rgb(255, 255, 255),
		Color.rgb(255, 0, 0), Color.rgb(255, 140, 0), Color.rgb(255, 255, 0), Color.rgb(0, 255, 0),
		Color.rgb(0, 191, 255), Color.rgb(106, 90, 205), Color.rgb(199, 21, 133), Color.rgb(255, 255, 255)
    };

    int initialAlpha = 190;
	int newAlpha = 138;
	int averageAlpha = (initialAlpha + newAlpha) / 2;
	int red = 0x2D;
	int green = 0x31;
	int blue = 0x33;

	int[] colors = {
        Color.argb(averageAlpha, red, green, blue), Color.argb(averageAlpha, red, green, blue),
        Color.argb(averageAlpha, red, green, blue), Color.argb(averageAlpha, red, green, blue),
        Color.argb(averageAlpha, red, green, blue), Color.argb(averageAlpha, red, green, blue),
        Color.argb(averageAlpha, red, green, blue), Color.argb(averageAlpha, red, green, blue),
        Color.argb(averageAlpha, red, green, blue), Color.argb(averageAlpha, red, green, blue),
        Color.argb(averageAlpha, red, green, blue), Color.argb(averageAlpha, red, green, blue)
	};

    float transparency = 1.0f;
    static int animationDuration = 540;
    static int valueAnimOne = 1;
    static int valueAnimZero = 0;
    float titleTextSize = 18.0f;
    float menuCornerRadius = 30.0f;
    byte strokeThickness = 3;
    int animationSpeed = 45000;
    String buttonTextColor = "#FFFFFFFF";
    String buttonPressedColor = "#35FFFFFF";
    String numberColor = "#FFFFFF";
	int textGlowColor = Color.WHITE;
	float textGlowRadius = 10.0f;
    String filePath = "/data/data/" + MainActivity.packageGame + "/files/Shader.txt";
    boolean firstClick = false, secondClick = false;
    Button btn;
    View mFloatingView;
    WindowManager mWindowManager;
    WindowManager.LayoutParams params;
    boolean serviceTestQueued;
    Button btnClose;
    LinearLayout buttonPanel, patchContainer;
    RelativeLayout collapsedView, expandedView, rootContainer;
    FrameLayout rootFrame;
    ImageView imageIcon;
    int savedWidth, savedHeight, minWidth, minHeight;
    TextView titleTextView;
    View.OnTouchListener floatingTouchListener;
    ValueAnimator colorAnimator;

    ArrayList<TextView> categoryTextViews = new ArrayList<>();

    native void Cases(int feature, int value, Context ctx);
    native void Views(Context ctx, TextView tx, TextView tx2);

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    public int onStartCommand(Intent intent, int i, int i2) {
        return Service.START_NOT_STICKY;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        instance = this;
        sharedPreferences = getSharedPreferences("config", Context.MODE_PRIVATE);
        enableAnimation = sharedPreferences.getBoolean("enableAnimation", false);
        enableBottomAnimation = sharedPreferences.getBoolean("enableBottomAnimation", false);
        enableBottom = sharedPreferences.getBoolean("enableBottom", false);
        if (!enableAnimation) {
            for (int i = 0; i < rainbowColors.length; i++) {
                rainbowColors[i] = Color.WHITE;
            }
        }
        String channelId = "overlay_channel";
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(channelId, "Overlay Service", NotificationManager.IMPORTANCE_LOW);
            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(channel);
        }
        Notification.Builder builder;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            builder = new Notification.Builder(this, channelId);
        } else {
            builder = new Notification.Builder(this);
        }

        Notification notification = builder
			.setContentTitle("Overlay Service")
			.setContentText("Chech has been running")
			.setSmallIcon(R.drawable.icon)
			.build();
        startForeground(1, notification);
        initFloating();
    }

    public static Animation fadeout() {
        AlphaAnimation alphaAnimation = new AlphaAnimation((float) valueAnimZero, (float) valueAnimOne);
        alphaAnimation.setDuration((long) animationDuration);
        return alphaAnimation;
    }

	private void glowText(TextView textView, int textColor) {
        int glowColor;
		if (textColor == Color.WHITE || textColor == Color.parseColor("#FFFFFFFF")) {
			glowColor = textGlowColor;
		} else {
			glowColor = textColor;
        }
        textView.setShadowLayer(textGlowRadius, 0f, 0f, glowColor);
	}

    public void initFloating() {
        Context baseContext = getBaseContext();
        rootFrame = new FrameLayout(baseContext);
        rootContainer = new RelativeLayout(baseContext);
        collapsedView = new RelativeLayout(baseContext);
        expandedView = new RelativeLayout(baseContext);
        patchContainer = new LinearLayout(baseContext);
        buttonPanel = new LinearLayout(baseContext);

        imageIcon = new ImageView(baseContext);
        imageIcon.setLayoutParams(new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));
        int iconSize = dp(50);
        imageIcon.getLayoutParams().height = iconSize;
        imageIcon.getLayoutParams().width = iconSize;
        imageIcon.requestLayout();
        imageIcon.setScaleType(ImageView.ScaleType.CENTER_CROP);
        imageIcon.setImageResource(R.drawable.icon);
        imageIcon.setImageAlpha(200);
        ((ViewGroup.MarginLayoutParams) imageIcon.getLayoutParams()).topMargin = dp(10);

        expandedView.setVisibility(View.GONE);
        expandedView.setAlpha(transparency);
        expandedView.setLayoutParams(new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

        titleTextView = new TextView(baseContext);
		glowText(titleTextView, Color.WHITE);
        titleTextView.setTypeface(Typeface.DEFAULT_BOLD);
        titleTextView.setGravity(Gravity.CENTER_HORIZONTAL);
        titleTextView.setTextSize(20f);
        titleTextView.setBackgroundColor(Color.TRANSPARENT);
        titleTextView.setTextSize(titleTextSize);
        titleTextView.setTextColor(Color.WHITE);
        titleTextView.setPadding(10, 22, 10, 22);
        titleTextView.setId(View.generateViewId());
        titleTextView.setClickable(true);
        titleTextView.setFocusable(true);

        LinearLayout.LayoutParams titleLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        titleLayoutParams.gravity = Gravity.CENTER_HORIZONTAL;
        titleTextView.setLayoutParams(titleLayoutParams);

        LinearLayout headerContainer = new LinearLayout(baseContext);
        headerContainer.setOrientation(LinearLayout.VERTICAL);
        headerContainer.setId(View.generateViewId());
        RelativeLayout.LayoutParams headerParams = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
        headerParams.addRule(RelativeLayout.ALIGN_PARENT_TOP);
        headerContainer.setLayoutParams(headerParams);
        headerContainer.addView(titleTextView);

        ScrollView scrollView = new ScrollView(baseContext);
        RelativeLayout.LayoutParams scrollViewParams = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
        scrollViewParams.addRule(RelativeLayout.BELOW, headerContainer.getId());
        scrollView.setLayoutParams(scrollViewParams);
        scrollView.setVerticalScrollBarEnabled(false);
        scrollView.setHorizontalScrollBarEnabled(false);

        patchContainer.setLayoutParams(new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));
        patchContainer.setOrientation(LinearLayout.VERTICAL);

        buttonPanel.setLayoutParams(new LinearLayout.LayoutParams(-2, -2));

        rootFrame.addView(rootContainer);
        rootContainer.addView(collapsedView);
        rootContainer.addView(expandedView);
        collapsedView.addView(imageIcon);
        scrollView.addView(patchContainer);
        expandedView.addView(headerContainer);
        expandedView.addView(scrollView);

        GradientDrawable backgroundDrawable = new GradientDrawable();
        backgroundDrawable.setCornerRadius(menuCornerRadius);
        if (enableBottom) {
            colors[11] = Color.argb(190, 255, 255, 255);
        }
        backgroundDrawable.setColors(colors);
        backgroundDrawable.setOrientation(GradientDrawable.Orientation.TOP_BOTTOM);
        expandedView.setBackground(backgroundDrawable);

        titleTextView.bringToFront();
        titleTextView.setOnLongClickListener(new View.OnLongClickListener() {
				@Override
				public boolean onLongClick(View v) {
					imageIcon.setImageAlpha(0);
					savedWidth = params.width;
					savedHeight = params.height;
					params.width = minWidth;
					params.height = minHeight;
					mWindowManager.updateViewLayout(mFloatingView, params);
					closeAllSubWindows();
					collapsedView.setVisibility(View.VISIBLE);
					expandedView.setVisibility(View.GONE);					
					firstClick = false;
					secondClick = false;
					return true;
				}
			});

        colorAnimator = ObjectAnimator.ofInt(titleTextView, "textColor", rainbowColors);

        final GradientDrawable strokeDrawable = new GradientDrawable();
        strokeDrawable.setShape(GradientDrawable.RECTANGLE);
        strokeDrawable.setCornerRadius(menuCornerRadius);
        strokeDrawable.setStroke(strokeThickness, Color.parseColor("#FFFFFF"));

        final int[] currentAnimationColor = {rainbowColors[0]};
        colorAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
				@Override
				public void onAnimationUpdate(ValueAnimator animation) {
					int animatedColor = (int) animation.getAnimatedValue();
					strokeDrawable.setStroke(strokeThickness, animatedColor);
					currentAnimationColor[0] = animatedColor;
					if (enableBottom && !enableBottomAnimation) {
						colors[11] = Color.argb(190, 255, 255, 255);
					} else if (enableBottom && enableBottomAnimation) {
						colors[11] = animatedColor;
					}
					((GradientDrawable) expandedView.getBackground()).setColors(colors);

					if (enableAnimation) {
						titleTextView.setShadowLayer(textGlowRadius, 0f, 0f, animatedColor);
					} else {
						titleTextView.setShadowLayer(textGlowRadius, 0f, 0f, Color.WHITE);
					}

					for (TextView categoryTextView : categoryTextViews) {
						if (enableAnimation) {
							categoryTextView.setTextColor(animatedColor);
							categoryTextView.setShadowLayer(textGlowRadius, 0f, 0f, animatedColor);
						} else {
							categoryTextView.setTextColor(Color.WHITE);
							categoryTextView.setShadowLayer(textGlowRadius, 0f, 0f, Color.WHITE);
						}
					}
				}
			});
        colorAnimator.setDuration(animationSpeed);
        colorAnimator.setEvaluator(new ArgbEvaluator());
        colorAnimator.setRepeatCount(ValueAnimator.INFINITE);
        colorAnimator.setRepeatMode(ValueAnimator.REVERSE);

        if (enableAnimation) {
            colorAnimator.start();
        }
        expandedView.setForeground(strokeDrawable);

        mFloatingView = rootFrame;

        int windowType = Build.VERSION.SDK_INT >= Build.VERSION_CODES.O ?
			WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY :
			WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;

        params = new WindowManager.LayoutParams(-2, -2,
												windowType,
												WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE |
												WindowManager.LayoutParams.FLAG_LAYOUT_IN_OVERSCAN |
												WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN |
												WindowManager.LayoutParams.FLAG_SPLIT_TOUCH,
												PixelFormat.TRANSPARENT);

        WindowManager.LayoutParams layoutParams4 = params;
        layoutParams4.gravity = 51;

        params.width = dp(60);
        params.height = dp(60);
        minWidth = dp(60);
        minHeight = dp(60);
        layoutParams4.x = 0;
        layoutParams4.y = 100;
        mWindowManager = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
        mWindowManager.addView(mFloatingView, params);

        savedWidth = params.width;
        savedHeight = params.height;

        floatingTouchListener = createFloatingTouchListener();

        mFloatingView.setOnTouchListener(floatingTouchListener);

        titleTextView.setOnClickListener(new View.OnClickListener() {
				@Override
				public void onClick(View view) {
					savedWidth = params.width;
					savedHeight = params.height;
					params.width = minWidth;
					params.height = minHeight;
					mWindowManager.updateViewLayout(mFloatingView, params);

					closeAllSubWindows();
					collapsedView.setVisibility(View.VISIBLE);
					expandedView.setVisibility(View.GONE);
					collapsedView.setAnimation(fadeout());				
					firstClick = false;
					secondClick = false;
				}
			});
        Views(this, titleTextView, null);
    }

	private View.OnTouchListener createFloatingTouchListener() {
        return new View.OnTouchListener() {
            private float initialTouchX;
            private float initialTouchY;
            private int initialX;
            private int initialY;
            private long touchStartTime;
            private boolean isDragging;

            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        initialX = params.x;
                        initialY = params.y;
                        initialTouchX = motionEvent.getRawX();
                        initialTouchY = motionEvent.getRawY();
                        touchStartTime = System.currentTimeMillis();
                        isDragging = false;
                        return true;

                    case MotionEvent.ACTION_MOVE:
                        float dx = motionEvent.getRawX() - initialTouchX;
                        float dy = motionEvent.getRawY() - initialTouchY;
                        if (Math.abs(dx) > 10 || Math.abs(dy) > 10) {
                            isDragging = true;
                        }

                        params.x = initialX + (int) dx;
                        params.y = initialY + (int) dy;
                        mWindowManager.updateViewLayout(mFloatingView, params);
                        return true;

                    case MotionEvent.ACTION_UP:
                        long touchDuration = System.currentTimeMillis() - touchStartTime;
                        if (touchDuration < CLICK_DURATION_THRESHOLD && !isDragging) {
                            if (secondClick) {
                                params.width = savedWidth;
                                params.height = savedHeight;
                                mWindowManager.updateViewLayout(mFloatingView, params);
                            } else {
                                params.width = dp(250);
                                params.height = dp(300);
                                mWindowManager.updateViewLayout(mFloatingView, params);
                            }
                            if (!firstClick) {
                                params.width = dp(250);
                                params.height = dp(300);
                                mWindowManager.updateViewLayout(mFloatingView, params);
                                firstClick = true;
                            }
                            if (collapsedView.getVisibility() == View.VISIBLE) {
                                params.width = savedWidth > minWidth ? savedWidth : dp(250);
                                params.height = savedHeight > minHeight ? savedHeight : dp(300);
                                mWindowManager.updateViewLayout(mFloatingView, params);
                                collapsedView.setVisibility(View.GONE);
                                expandedView.setVisibility(View.VISIBLE);
                                expandedView.setAnimation(fadeout());
                                imageIcon.setImageAlpha(200);
                            }
                            return true;
                        }
                        return true;

                    default:
                        return false;
                }
            }
        };
    }

    public void beginSubWindow(final String name) {
        subWindowItems.put(name, new java.util.ArrayList<>());
        currentSubWindow = name;

        final Button toggle = new Button(Ui.this);
        glowText(toggle, Color.WHITE);
        toggle.setText("▶ " + name + " ▶");
        toggle.setAllCaps(false);
        toggle.setTextColor(Color.parseColor(buttonTextColor));
        toggle.setTextSize(14.7f);
        toggle.setGravity(17);
        toggle.setBackgroundColor(Color.parseColor("#00FFFFFF"));
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
			LinearLayout.LayoutParams.MATCH_PARENT,
			LinearLayout.LayoutParams.WRAP_CONTENT);
        toggle.setLayoutParams(lp);

        final boolean[] isOpen = {false};
        toggle.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (isOpen[0]) {
                        closeSubWindow(name, toggle, isOpen);
                    } else {
                        openSubWindow(name, toggle, isOpen);
                        toggle.setText("▼ " + name + " ▼");
                        toggle.setBackgroundColor(Color.parseColor(buttonPressedColor));
                        isOpen[0] = true;
                    }
                }
            });

        subWindowToggles.put(name, toggle);
        LinearLayout.LayoutParams toggleParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
        toggle.setLayoutParams(toggleParams);
        patchContainer.addView(toggle);
    }

    public void endSubWindow(String name) {
        currentSubWindow = null;
    }

    private final java.util.Map<String, View> subWindowViews = new java.util.LinkedHashMap<>();
    private final java.util.Map<String, WindowManager.LayoutParams> subWindowParams = new java.util.LinkedHashMap<>();
    private final java.util.Map<String, LinearLayout> subWindowRootCache = new java.util.LinkedHashMap<>();
    private final java.util.Map<String, WindowManager.LayoutParams> subWindowParamsCache = new java.util.LinkedHashMap<>();

    private void openSubWindow(final String name, final Button toggle, final boolean[] isOpen) {
        if (subWindowViews.containsKey(name)) return;

        int windowType = Build.VERSION.SDK_INT >= Build.VERSION_CODES.O
			? WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY
			: WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;

        final LinearLayout root;
        final WindowManager.LayoutParams subParams;

        if (subWindowRootCache.containsKey(name)) {          
            root = subWindowRootCache.get(name);
            subParams = subWindowParamsCache.get(name);         
            subParams.x = params.x + params.width + dp(10);
            subParams.y = params.y;
        } else {          
            int[] subColors = new int[colors.length];
            for (int i = 0; i < subColors.length; i++) {
                subColors[i] = Color.argb(averageAlpha, red, green, blue);
            }
            GradientDrawable dialogBg = new GradientDrawable();
            dialogBg.setCornerRadius(menuCornerRadius);
            dialogBg.setColors(subColors);
            dialogBg.setOrientation(GradientDrawable.Orientation.TOP_BOTTOM);
            GradientDrawable dialogStroke = new GradientDrawable();
            dialogStroke.setShape(GradientDrawable.RECTANGLE);
            dialogStroke.setCornerRadius(menuCornerRadius);
            dialogStroke.setStroke(strokeThickness, Color.WHITE);

            root = new LinearLayout(Ui.this);
            root.setOrientation(LinearLayout.VERTICAL);
            root.setPadding(0, 0, 0, 16);
            root.setBackground(dialogBg);
            root.setForeground(dialogStroke);

            final TextView titleBtn = new TextView(Ui.this);
            glowText(titleBtn, Color.WHITE);
            titleBtn.setTypeface(Typeface.DEFAULT_BOLD);
            titleBtn.setGravity(Gravity.CENTER_HORIZONTAL);
            titleBtn.setTextSize(titleTextSize);
            titleBtn.setTextColor(Color.WHITE);
            titleBtn.setPadding(10, 22, 10, 22);
            titleBtn.setText(name);
            titleBtn.setClickable(true);
            titleBtn.setFocusable(true);

            ScrollView scroll = new ScrollView(Ui.this);
            scroll.setVerticalScrollBarEnabled(false);
            LinearLayout itemsContainer = new LinearLayout(Ui.this);
            itemsContainer.setOrientation(LinearLayout.VERTICAL);
            scroll.addView(itemsContainer);
            root.addView(titleBtn);
            root.addView(scroll);

            subWindowContainers.put(name, itemsContainer);

            java.util.List<Runnable> items = subWindowItems.get(name);
            if (items != null) {
                for (Runnable r : items) r.run();
            }

            subParams = new WindowManager.LayoutParams(
				params.width,
				params.height,
				windowType,
				WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE |
				WindowManager.LayoutParams.FLAG_LAYOUT_IN_OVERSCAN |
				WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN |
				WindowManager.LayoutParams.FLAG_SPLIT_TOUCH,
				PixelFormat.TRANSPARENT
            );
            subParams.gravity = Gravity.TOP | Gravity.LEFT;
            subParams.x = params.x + params.width + dp(10);
            subParams.y = params.y;
            subWindowRootCache.put(name, root);
            subWindowParamsCache.put(name, subParams);

            titleBtn.setOnTouchListener(new View.OnTouchListener() {
					private float initTouchX, initTouchY;
					private int initX, initY;
					private long touchStart;
					private boolean dragging;

					@Override
					public boolean onTouch(View v, MotionEvent e) {
						switch (e.getAction()) {
							case MotionEvent.ACTION_DOWN:
								initX = subParams.x;
								initY = subParams.y;
								initTouchX = e.getRawX();
								initTouchY = e.getRawY();
								touchStart = System.currentTimeMillis();
								dragging = false;
								return true;
							case MotionEvent.ACTION_MOVE:
								float dx = e.getRawX() - initTouchX;
								float dy = e.getRawY() - initTouchY;
								if (Math.abs(dx) > 5 || Math.abs(dy) > 5) dragging = true;
								subParams.x = initX + (int) dx;
								subParams.y = initY + (int) dy;
								mWindowManager.updateViewLayout(root, subParams);
								return true;
							case MotionEvent.ACTION_UP:
								if (!dragging && System.currentTimeMillis() - touchStart < CLICK_DURATION_THRESHOLD) {
									closeSubWindow(name, toggle, isOpen);
								}
								return true;
						}
						return false;
					}
				});
        }

        subWindowParams.put(name, subParams);
        subWindowViews.put(name, root);
        mWindowManager.addView(root, subParams);
    }

    private void closeAllSubWindows() {
        for (java.util.Map.Entry<String, View> entry : new java.util.ArrayList<>(subWindowViews.entrySet())) {
            String n = entry.getKey();
            View v = entry.getValue();
            if (v != null) {
                try { mWindowManager.removeView(v); } catch (Exception ignored) {}
            }
            subWindowParams.remove(n);
            subWindowContainers.remove(n);
            Button toggle = subWindowToggles.get(n);
            if (toggle != null) {
                toggle.setText("▶ " + n + " ▶");
                toggle.setBackgroundColor(Color.parseColor("#00FFFFFF"));
            }
        }
        subWindowViews.clear();
    }

    private void closeSubWindow(String name, Button toggle, boolean[] isOpen) {
        View v = subWindowViews.remove(name);
        if (v != null) {
            mWindowManager.removeView(v);
        }
        subWindowParams.remove(name);
        subWindowContainers.remove(name);
        if (toggle != null) {
            toggle.setText("▶ " + name + " ▶");
            toggle.setBackgroundColor(Color.parseColor("#00FFFFFF"));
        }
        if (isOpen != null) isOpen[0] = false;
    }

    private void closeSubWindow(String name) {
        View v = subWindowViews.remove(name);
        if (v != null) mWindowManager.removeView(v);
        subWindowParams.remove(name);
        subWindowContainers.remove(name);
    }

    public void addCategory(final String text) {
        if (currentSubWindow != null) {
            final String target = currentSubWindow;
            java.util.List<Runnable> items = subWindowItems.get(target);
            if (items != null) {
                items.add(new Runnable() {
						@Override
						public void run() {
							LinearLayout container = subWindowContainers.get(target);
							if (container == null) return;
							TextView tv = new TextView(Ui.this);
							if (enableAnimation) {
								glowText(tv, rainbowColors[0]);
							} else {
								glowText(tv, Color.WHITE);
							}
							tv.setBackgroundColor(Color.parseColor("#00FFFFFF"));
							tv.setText(text);
							tv.setGravity(16);
							tv.setTextSize(13f);
							tv.setTextColor(enableAnimation ? rainbowColors[0] : Color.WHITE);
							tv.setTypeface(null, Typeface.BOLD);
							tv.setPadding(12, 3, 12, 3);
							categoryTextViews.add(tv);
							container.addView(tv);
						}
					});
            }
            return;
        }

        TextView categoryTextView = new TextView(this);
		if (enableAnimation) {
			glowText(categoryTextView, rainbowColors[0]);
		} else {
			glowText(categoryTextView, Color.WHITE);
		}
        categoryTextView.setBackgroundColor(Color.parseColor("#00FFFFFF"));
        categoryTextView.setText(text);
        categoryTextView.setGravity(16);
        categoryTextView.setTextSize(13f);
        categoryTextView.setTextColor(enableAnimation ? rainbowColors[0] : Color.WHITE);
        categoryTextView.setTypeface(null, Typeface.BOLD);
        categoryTextView.setPadding(12, 3, 12, 3);
        categoryTextViews.add(categoryTextView);
        LinearLayout.LayoutParams catParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
        categoryTextView.setLayoutParams(catParams);
        patchContainer.addView(categoryTextView);
    }

	public void addSeekBar(final String feature, final int progress, int max, final InterfaceInt interInt) {
        if (currentSubWindow != null) {
            final String target = currentSubWindow;
            final int maxVal = max;
            java.util.List<Runnable> items = subWindowItems.get(target);
            if (items != null) {
                items.add(new Runnable() {
						@Override
						public void run() {
							LinearLayout container = subWindowContainers.get(target);
							if (container == null) return;

							LinearLayout seekBarContainer = new LinearLayout(Ui.this);
							seekBarContainer.setPadding(10, 5, 0, 5);
							seekBarContainer.setOrientation(LinearLayout.VERTICAL);
							seekBarContainer.setGravity(Gravity.CENTER);
							seekBarContainer.setLayoutParams(new LinearLayout.LayoutParams(-1, -2));
							seekBarContainer.setBackgroundColor(Color.TRANSPARENT);

							final TextView seekBarLabel = new TextView(Ui.this);
							glowText(seekBarLabel, Color.WHITE);
							seekBarLabel.setText(Html.fromHtml("<font face='roboto'>" + feature + ": <font color='#FFFFFF'>" + progress + "</font>"));
							seekBarLabel.setTextColor(Color.WHITE);
							seekBarLabel.setTextSize(14);

							final SeekBar seekBar = new SeekBar(Ui.this);
							seekBar.setMax(maxVal);
							seekBar.setProgressDrawable(getResources().getDrawable(R.drawable.seekbar_progress, getTheme()));
							seekBar.setThumb(getResources().getDrawable(R.drawable.seekbar_thumb, getTheme()));
							seekBar.setPadding(25, 10, 35, 10);
							LinearLayout.LayoutParams lp2 = new LinearLayout.LayoutParams(-1, LinearLayout.LayoutParams.WRAP_CONTENT);
							lp2.bottomMargin = 10;
							seekBar.setLayoutParams(lp2);
							seekBar.setProgress(progress);

							seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
									public void onStartTrackingTouch(SeekBar s) {}
									public void onStopTrackingTouch(SeekBar s) {}
									public void onProgressChanged(SeekBar s, int i, boolean z) {
										if (i < progress) {
											s.setProgress(progress);
											interInt.OnWrite(progress);
											seekBarLabel.setText(Html.fromHtml("<font face='roboto'>" + feature + ": <font color='#FFFFFF'>" + progress + "</font>"));
											return;
										}
										interInt.OnWrite(i);
										seekBarLabel.setText(Html.fromHtml("<font face='roboto'>" + feature + ": <font color='#FFFFFF'>" + i + "</font></font>"));
									}
								});

							seekBarContainer.addView(seekBarLabel);
							seekBarContainer.addView(seekBar);
							container.addView(seekBarContainer);
						}
					});
            }
            return;
        }

		LinearLayout seekBarContainer = new LinearLayout(this);
		LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(-1, -2);
		seekBarContainer.setPadding(10, 5, 0, 5);
		seekBarContainer.setOrientation(LinearLayout.VERTICAL);
		seekBarContainer.setGravity(Gravity.CENTER);
		seekBarContainer.setLayoutParams(layoutParams);
		seekBarContainer.setBackgroundColor(Color.TRANSPARENT);

		final TextView seekBarLabel = new TextView(this);
		glowText(seekBarLabel, Color.WHITE);
		seekBarLabel.setText(Html.fromHtml("<font face='roboto'>" + feature + ": <font color='#FFFFFF'>" + progress + "</font>"));
		seekBarLabel.setTextColor(Color.WHITE);
		seekBarLabel.setTextSize(14);

		final SeekBar seekBar = new SeekBar(this);
		seekBar.setMax(max);
		seekBar.setProgressDrawable(getResources().getDrawable(R.drawable.seekbar_progress, getTheme()));
		seekBar.setThumb(getResources().getDrawable(R.drawable.seekbar_thumb, getTheme()));
		seekBar.setPadding(25, 10, 35, 10);
		LinearLayout.LayoutParams layoutParams2 = new LinearLayout.LayoutParams(-1,  LinearLayout.LayoutParams.WRAP_CONTENT);
		layoutParams2.bottomMargin = 10;
		seekBar.setLayoutParams(layoutParams2);

		seekBar.setProgress(progress);
		seekBar.setMax(max);
		seekBar.setOnTouchListener(new View.OnTouchListener() {
				@Override
				public boolean onTouch(View view, MotionEvent motionEvent) {
					if (motionEvent.getAction() == MotionEvent.ACTION_MOVE) {
						if (seekBar.getProgress() < progress) {
							seekBar.setProgress(progress);
							return true;
						}
					}
					return false;
				}
			});

		final TextView finalSeekBarLabel = seekBarLabel;
		seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
				public void onStartTrackingTouch(SeekBar seekBar) {
				}
				public void onStopTrackingTouch(SeekBar seekBar) {
				}
				int currentValue;
				public void onProgressChanged(SeekBar seekBar, int i, boolean z) {
					currentValue = i;
					if (i < progress) {
						seekBar.setProgress(progress);
						interInt.OnWrite(progress);
						TextView textView = finalSeekBarLabel;
						textView.setText(Html.fromHtml("<font face='roboto'>" + feature + ": <font color='#FFFFFF'>" + progress + "</font>"));
						return;
					}
					interInt.OnWrite(i);
					seekBarLabel.setText(Html.fromHtml("<font face='roboto'>" + feature + ": <font color='#FFFFFF'>" + String.valueOf(i) + "</font></font>"));
				}
			});

		seekBarContainer.addView(seekBarLabel);
		seekBarContainer.addView(seekBar);
        LinearLayout.LayoutParams sbParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
        seekBarContainer.setLayoutParams(sbParams);
        patchContainer.addView(seekBarContainer);
	}

    public void addButton(String feature, final InterfaceBtn interfaceBtn) {
        if (currentSubWindow != null) {
            final String target = currentSubWindow;
            final String feat = feature;
            java.util.List<Runnable> items = subWindowItems.get(target);
            if (items != null) {
                items.add(new Runnable() {
						@Override
						public void run() {
							LinearLayout container = subWindowContainers.get(target);
							if (container == null) return;

							final Button button = new Button(Ui.this);
							glowText(button, Color.WHITE);
							LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                                LinearLayout.LayoutParams.MATCH_PARENT,
                                LinearLayout.LayoutParams.WRAP_CONTENT);
							button.setLayoutParams(lp);
							button.setPadding(0, 0, 0, 0);
							button.setTextColor(Color.parseColor(buttonTextColor));
							button.setTextSize(14.7f);
							button.setGravity(17);
							button.setText(feat + " ");
							button.setAllCaps(false);
							button.setBackgroundColor(Color.parseColor("#00FFFFFF"));
							final boolean[] isActive = {true};
							button.setOnClickListener(new View.OnClickListener() {
									@Override
									public void onClick(View v) {
										interfaceBtn.OnWrite();
										if (isActive[0]) {
											button.setText(feat);
											button.setTextSize(15.5f);
											button.setBackgroundColor(Color.parseColor(buttonPressedColor));
											isActive[0] = false;
										} else {
											button.setText(feat);
											button.setTextSize(14.7f);
											button.setBackgroundColor(Color.parseColor("#00FFFFFF"));
											isActive[0] = true;
										}
									}
								});
							container.addView(button);
						}
					});
            }
            return;
        }

        final Button button = new Button(this);
        LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
		glowText(button, Color.WHITE);
        layoutParams.setMargins(0, 0, 0, 0);
        button.setLayoutParams(layoutParams);

        button.setPadding(0, 0, 0, 0);
        button.setTextColor(Color.parseColor(buttonTextColor));
        button.setTextSize(14.7f);
        button.setGravity(17);
        button.setText(feature + " ");
        button.setAllCaps(false);
        button.setBackgroundColor(Color.parseColor("#00FFFFFF"));
        final String buttonText = feature;
        final boolean[] isActive = {true};

        button.setOnClickListener(new View.OnClickListener() {
				@Override
				public void onClick(View v) {
					interfaceBtn.OnWrite();
					if (isActive[0]) {
						button.setText(buttonText);
						button.setTextSize(15.5f);
						button.setBackgroundColor(Color.parseColor(buttonPressedColor));
						isActive[0] = false;
					} else {
						button.setText(buttonText);
						button.setTextSize(14.7f);
						button.setBackgroundColor(Color.parseColor("#00FFFFFF"));
						isActive[0] = true;
					}
				}
			});
        patchContainer.addView(button);
    }

    public void addButtonOnce(String feature, final InterfaceBtn interfaceBtn) {
        if (currentSubWindow != null) {
            final String target = currentSubWindow;
            final String feat = feature;
            java.util.List<Runnable> items = subWindowItems.get(target);
            if (items != null) {
                items.add(new Runnable() {
						@Override
						public void run() {
							LinearLayout container = subWindowContainers.get(target);
							if (container == null) return;

							final Button button = new Button(Ui.this);
							glowText(button, Color.WHITE);
							LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                                LinearLayout.LayoutParams.MATCH_PARENT,
                                LinearLayout.LayoutParams.WRAP_CONTENT);
							button.setLayoutParams(lp);
							button.setPadding(0, 0, 0, 0);
							button.setTextColor(Color.parseColor(buttonTextColor));
							button.setTextSize(14.7f);
							button.setGravity(17);
							button.setText(feat + " ");
							button.setAllCaps(false);
							button.setBackgroundColor(Color.parseColor("#00FFFFFF"));
							button.setOnClickListener(new View.OnClickListener() {
									private boolean isActive = true;
									@Override
									public void onClick(View v) {
										if (isActive) {
											interfaceBtn.OnWrite();
											button.setText(feat);
											button.setTextSize(15.5f);
											button.setBackgroundColor(Color.parseColor(buttonPressedColor));
											new Handler().postDelayed(new Runnable() {
													@Override
													public void run() {
														button.setText(feat);
														button.setTextSize(14.7f);
														button.setBackgroundColor(Color.parseColor("#00FFFFFF"));
													}
												}, 250);
										}
									}
								});
							container.addView(button);
						}
					});
            }
            return;
        }

        final Button button = new Button(this);
        LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
		glowText(button, Color.WHITE);
        layoutParams.setMargins(0, 0, 0, 0);
        button.setLayoutParams(layoutParams);
        button.setPadding(0, 0, 0, 0);
        button.setTextColor(Color.parseColor(buttonTextColor));
        button.setTextSize(14.7f);
        button.setGravity(17);
        button.setText(feature + " ");
        button.setAllCaps(false);
        button.setBackgroundColor(Color.parseColor("#00FFFFFF"));
        final String buttonText = feature;
        button.setOnClickListener(new View.OnClickListener() {
				private boolean isActive = true;

				public void onClick(View v) {
					if (isActive) {
						interfaceBtn.OnWrite();
						button.setText(buttonText);
						button.setTextSize(15.5f);
						button.setBackgroundColor(Color.parseColor(buttonPressedColor));
						new Handler().postDelayed(new Runnable() {
								@Override
								public void run() {
									button.setText(buttonText);
									button.setTextSize(14.7f);
									button.setBackgroundColor(Color.parseColor("#00FFFFFF"));
								}
							}, 250);
					}
				}
			});
        patchContainer.addView(button);
    }

    public void addEditText(final String featName, final InterfaceStr interfaceStr) {
        LinearLayout linearLayout = new LinearLayout(this);
        LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
        layoutParams.setMargins(7, 5, 7, 5);
        linearLayout.setOrientation(LinearLayout.VERTICAL);
        linearLayout.setGravity(Gravity.CENTER);
        linearLayout.setLayoutParams(layoutParams);

        final Button button = new Button(this);
		glowText(button, Color.WHITE);
        String string = "";
        button.setText(Html.fromHtml(featName + ": <font color='" + numberColor + "'>" + string + "</font>"));

        button.setAllCaps(false);
        button.setLayoutParams(layoutParams);
        button.setBackgroundColor(Color.parseColor("#00000000"));
        button.setTextColor(Color.parseColor("#FFFFFFFF"));
        button.setOnClickListener(new View.OnClickListener() {
				@Override
				public void onClick(View view) {
					AlertDialog.Builder alertName = new AlertDialog.Builder(Ui.this);

					final EditText editText = new EditText(Ui.this);
					editText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
							@Override
							public void onFocusChange(View v, boolean hasFocus) {
								InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
								if (hasFocus) {
									imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, InputMethodManager.HIDE_IMPLICIT_ONLY);
								} else {
									imm.toggleSoftInput(InputMethodManager.HIDE_IMPLICIT_ONLY, 0);
								}
							}
						});
					editText.requestFocus();

					alertName.setTitle("Input shader name");
					alertName.setView(editText);
					LinearLayout layoutName = new LinearLayout(Ui.this);
					layoutName.setOrientation(LinearLayout.VERTICAL);
					layoutName.addView(editText);
					alertName.setView(layoutName);

					alertName.setPositiveButton("OK", new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
								String str = editText.getText().toString();
								button.setText(Html.fromHtml(featName + ": <font color='" + numberColor + "'>" + str + "</font>"));
								interfaceStr.OnWrite(str);
								editText.setFocusable(false);
							}
						});

					alertName.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
								InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
								imm.toggleSoftInput(InputMethodManager.HIDE_IMPLICIT_ONLY, 0);
							}
						});

					AlertDialog dialog = alertName.create();
					if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
						dialog.getWindow().setType(WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY);
					} else {
						dialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
					}

					dialog.show();
				}
			});

        linearLayout.addView(button);
        patchContainer.addView(linearLayout);
    }

	public void addSelector(final String featName, final String[] items, final InterfaceInt interfaceInt) {
		LinearLayout linearLayout = new LinearLayout(this);
		LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
			LinearLayout.LayoutParams.MATCH_PARENT,
			LinearLayout.LayoutParams.WRAP_CONTENT
		);
		layoutParams.setMargins(7, 5, 7, 5);
		linearLayout.setOrientation(LinearLayout.VERTICAL);
		linearLayout.setGravity(Gravity.CENTER);
		linearLayout.setLayoutParams(layoutParams);

		final Button button = new Button(this);
		glowText(button, Color.WHITE);

		button.setText(Html.fromHtml(featName + ": <font color='" + numberColor + "'>" + items[0] + "</font>"));
		button.setAllCaps(false);
		button.setLayoutParams(layoutParams);
		button.setBackgroundColor(Color.parseColor("#00000000"));
		button.setTextColor(Color.parseColor("#FFFFFFFF"));

		button.setOnClickListener(new View.OnClickListener() {
				@Override
				public void onClick(View view) {
					AlertDialog.Builder builder = new AlertDialog.Builder(Ui.this);
					builder.setTitle("Select " + featName);
					builder.setAdapter(new ArrayAdapter<>(Ui.this, android.R.layout.simple_list_item_1, items), new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int which) {
								button.setText(Html.fromHtml(featName + ": <font color='" + numberColor + "'>" + items[which] + "</font>"));
								interfaceInt.OnWrite(which);
							}
						});

					AlertDialog dialog = builder.create();
					if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
						dialog.getWindow().setType(WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY);
					} else {
						dialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
					}
					dialog.show();
				}
			});

		linearLayout.addView(button);
		patchContainer.addView(linearLayout);
	}

	public void addShaderSelector(final String featName, final InterfaceStr interfaceStr) {
		LinearLayout linearLayout = new LinearLayout(this);
		LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
			LinearLayout.LayoutParams.MATCH_PARENT,
			LinearLayout.LayoutParams.WRAP_CONTENT
		);
		layoutParams.setMargins(7, 5, 7, 5);
		linearLayout.setOrientation(LinearLayout.VERTICAL);
		linearLayout.setGravity(Gravity.CENTER);
		linearLayout.setLayoutParams(layoutParams);

		final Button button = new Button(this);
		glowText(button, Color.WHITE);
		button.setText(Html.fromHtml(featName + ": <font color='" + numberColor + "'>Select...</font>"));
		button.setAllCaps(false);
		button.setLayoutParams(layoutParams);
		button.setBackgroundColor(Color.parseColor("#00000000"));
		button.setTextColor(Color.parseColor("#FFFFFFFF"));
		shaderAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, shaderList);

		button.setOnClickListener(new View.OnClickListener() {
				@Override
				public void onClick(View view) {
					if (shaderList.isEmpty()) {
						return;
					}

					AlertDialog.Builder builder = new AlertDialog.Builder(Ui.this);
					builder.setTitle("Select Shader");
					builder.setAdapter(shaderAdapter, new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int which) {
								String selected = shaderList.get(which);
								button.setText(Html.fromHtml(featName + ": <font color='" + numberColor + "'>" + selected + "</font>"));
								interfaceStr.OnWrite(selected);
							}
						});

					AlertDialog dialog = builder.create();
					if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
						dialog.getWindow().setType(WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY);
					} else {
						dialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
					}
					dialog.show();
				}
			});

		linearLayout.addView(button);
		patchContainer.addView(linearLayout);
	}

	public void updateShaderList(final String shaderName) {
		new Handler(Looper.getMainLooper()).post(new Runnable() {
				@Override
				public void run() {
					if (!shaderList.contains(shaderName)) {
						shaderList.add(shaderName);
						if (shaderAdapter != null) {
							shaderAdapter.notifyDataSetChanged();
						}
					}
				}
			});
	}

    public boolean isViewCollapsed() {
        return mFloatingView == null || collapsedView.getVisibility() == View.VISIBLE;
    }

    private int dp(int i) {
        return (int) TypedValue.applyDimension(1, (float) i, getResources().getDisplayMetrics());
    }

    public InterfaceInt INT(final int z1) {
        return new InterfaceInt() {
            @Override
            public void OnWrite(int i) {
                Cases(z1, i, Ui.this);
            }
        };
    }

    public InterfaceBool BOOL(final int z2) {
        return new InterfaceBool() {
            @Override
            public void OnWrite(boolean z) {
                Cases(z2, 0, Ui.this);
            }
        };
    }

    public InterfaceBtn BTN(final int i1) {
        return new InterfaceBtn() {
            @Override
            public void OnWrite() {
                Cases(i1, 0, Ui.this);
            }
        };
    }

    public InterfaceStr STR(final int z3) {
		return new InterfaceStr() {
			@Override
			public void OnWrite(String s) {
				StringCases(z3, s);
			}
		};
	}

    private interface InterfaceBtn {
        void OnWrite();
    }

    private interface InterfaceInt {
        void OnWrite(int i);
    }

    private interface InterfaceBool {
        void OnWrite(boolean z);
    }

    private interface InterfaceStr {
        void OnWrite(String s);
    }

    public void applyAnimationSetting(boolean enabled) {
        if (!enabled) {
            if (colorAnimator != null) colorAnimator.cancel();
            new Handler(Looper.getMainLooper()).post(new Runnable() {
					@Override
					public void run() {
						if (titleTextView != null) {
							titleTextView.setTextColor(Color.WHITE);
							titleTextView.setShadowLayer(textGlowRadius, 0f, 0f, Color.WHITE);
						}
						for (TextView categoryTextView : categoryTextViews) {
							categoryTextView.setTextColor(Color.WHITE);
							categoryTextView.setShadowLayer(textGlowRadius, 0f, 0f, Color.WHITE);
						}
						GradientDrawable strokeDrawable = (GradientDrawable) expandedView.getForeground();
						if (strokeDrawable != null) strokeDrawable.setStroke(strokeThickness, Color.WHITE);
					}
				});
        } else {
            int[] original = {
                Color.rgb(255,0,0), Color.rgb(255,140,0), Color.rgb(255,255,0), Color.rgb(0,255,0),
                Color.rgb(0,191,255), Color.rgb(106,90,205), Color.rgb(199,21,133), Color.rgb(255,255,255),
                Color.rgb(255,0,0), Color.rgb(255,140,0), Color.rgb(255,255,0), Color.rgb(0,255,0),
                Color.rgb(0,191,255), Color.rgb(106,90,205), Color.rgb(199,21,133), Color.rgb(255,255,255)
            };
            System.arraycopy(original, 0, rainbowColors, 0, rainbowColors.length);
            if (colorAnimator != null) colorAnimator.cancel();
            colorAnimator = ObjectAnimator.ofInt(titleTextView, "textColor", rainbowColors);
            colorAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
					@Override
					public void onAnimationUpdate(ValueAnimator animation) {
						int animatedColor = (int) animation.getAnimatedValue();
						GradientDrawable strokeDrawable = (GradientDrawable) expandedView.getForeground();
						if (strokeDrawable != null) strokeDrawable.setStroke(strokeThickness, animatedColor);
						if (enableBottom && !enableBottomAnimation) {
							colors[11] = Color.argb(190, 255, 255, 255);
						} else if (enableBottom && enableBottomAnimation) {
							colors[11] = animatedColor;
						}
						((GradientDrawable) expandedView.getBackground()).setColors(colors);
						titleTextView.setShadowLayer(textGlowRadius, 0f, 0f, animatedColor);
						for (TextView categoryTextView : categoryTextViews) {
							categoryTextView.setTextColor(animatedColor);
							categoryTextView.setShadowLayer(textGlowRadius, 0f, 0f, animatedColor);
						}
					}
				});
            colorAnimator.setDuration(animationSpeed);
            colorAnimator.setEvaluator(new ArgbEvaluator());
            colorAnimator.setRepeatCount(ValueAnimator.INFINITE);
            colorAnimator.setRepeatMode(ValueAnimator.REVERSE);
            colorAnimator.start();
        }
    }

    public void applyBottomSetting(boolean enabled) {
        if (expandedView == null) return;
        GradientDrawable bg = (GradientDrawable) expandedView.getBackground();
        if (enabled) {
            colors[11] = Color.argb(190, 255, 255, 255);
        } else {
            colors[11] = Color.argb(averageAlpha, red, green, blue);
        }
        bg.setColors(colors);
    }

    public void onDestroy() {
        super.onDestroy();
        instance = null;
        for (View subView : subWindowViews.values()) {
            try { mWindowManager.removeView(subView); } catch (Exception ignored) {}
        }
        subWindowViews.clear();
        subWindowRootCache.clear();
        subWindowParamsCache.clear();
        if (mWindowManager != null && mFloatingView != null) {
            mWindowManager.removeView(mFloatingView);
            mFloatingView = null;
			System.exit(0);
        }
        if (colorAnimator != null) {
            colorAnimator.cancel();
        }
    }
}




