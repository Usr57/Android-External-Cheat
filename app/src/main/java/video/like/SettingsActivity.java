package video.like;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.view.View;
import android.content.res.ColorStateList;


public class SettingsActivity extends Activity {

    private SharedPreferences sharedPreferences;
    CheckBox resizeHandleCb, rgbCb, bottomRGBCb, bottomCb;
    boolean bottomFlag, rgbFlag;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        sharedPreferences = getSharedPreferences("config", Context.MODE_PRIVATE);

        rgbCb = findViewById(R.id.RGBCb);
        bottomRGBCb = findViewById(R.id.BottomRGBCb);
        bottomCb = findViewById(R.id.BottomCb);

        setCheckboxColor(rgbCb);
        setCheckboxColor(bottomRGBCb);
        setCheckboxColor(bottomCb);

        

        boolean enableAnimation = sharedPreferences.getBoolean("enableAnimation", false);
        rgbFlag = sharedPreferences.getBoolean("enableAnimation", false);
        rgbCb.setChecked(enableAnimation);

        boolean enableBottomAnimation = sharedPreferences.getBoolean("enableBottomAnimation", false);
        bottomRGBCb.setChecked(enableBottomAnimation);

        boolean enableBottom = sharedPreferences.getBoolean("enableBottom", false);
        bottomFlag = sharedPreferences.getBoolean("enableBottom", false);
        bottomCb.setChecked(enableBottom);


        updateBottomRGBCbVisibility();       

		rgbCb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
				@Override
				public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
					SharedPreferences.Editor editor = sharedPreferences.edit();
					editor.putBoolean("enableAnimation", isChecked);
					editor.apply();
					rgbFlag = isChecked;
					Ui.enableAnimation = isChecked;
					Ui instance = Ui.getInstance();
					if (instance != null) instance.applyAnimationSetting(isChecked);
					updateBottomRGBCbVisibility();
				}
			});

		bottomCb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
				@Override
				public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
					SharedPreferences.Editor editor = sharedPreferences.edit();
					editor.putBoolean("enableBottom", isChecked);
					editor.apply();
					bottomFlag = isChecked;
					Ui.enableBottom = isChecked;
					Ui instance = Ui.getInstance();
					if (instance != null) instance.applyBottomSetting(isChecked);
					updateBottomRGBCbVisibility();
				}
			});

		bottomRGBCb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
				@Override
				public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
					SharedPreferences.Editor editor = sharedPreferences.edit();
					editor.putBoolean("enableBottomAnimation", isChecked);
					editor.apply();
					Ui.enableBottomAnimation = isChecked;
				}
			});
    }

    void updateBottomRGBCbVisibility() {
        if (rgbFlag && bottomFlag) {
            bottomRGBCb.setVisibility(View.VISIBLE);
        } else {
            bottomRGBCb.setVisibility(View.GONE);
        }
    }

  
	@Override
	public void onBackPressed() {
		finish();
	}

    private void setCheckboxColor(CheckBox checkBox) {
        int[][] states = new int[][]{
			new int[]{android.R.attr.state_checked},
			new int[]{-android.R.attr.state_checked}
        };

        int[] colors = new int[]{
			getResources().getColor(R.color.white),
			getResources().getColor(R.color.white)
        };

        ColorStateList colorStateList = new ColorStateList(states, colors);
        checkBox.setButtonTintList(colorStateList);
    }
}
