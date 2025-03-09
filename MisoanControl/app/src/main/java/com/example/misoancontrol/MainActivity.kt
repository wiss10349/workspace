package com.example.misoancontrol
import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.*
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanResult
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import java.util.*
import androidx.activity.result.contract.ActivityResultContracts //追加
import com.example.misoancontrol.R

class MainActivity : AppCompatActivity() {

    private val bluetoothAdapter: BluetoothAdapter? by lazy {
        (getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager).adapter
    }
    private val targetDeviceAddress = "XX:XX:XX:XX:XX:XX" // ターゲットデバイスのMACアドレス
    private val targetDeviceUUID = UUID.fromString("12345678-1234-1234-1234-1234567890AB")
    private val serviceUUID = UUID.fromString("12345678-1234-1234-1234-1234567890AC")
    private val characteristicUUID = UUID.fromString("12345678-1234-1234-1234-1234567890AD")
    private var bluetoothGatt: BluetoothGatt? = null
    private lateinit var statusTextView: TextView
    private lateinit var receiveTextView: TextView

    private val REQUEST_LOCATION_PERMISSION = 2

    private val enableBtLauncher = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
        if (result.resultCode == RESULT_OK) {
            checkLocationPermission()
        } else {
            statusTextView.text = "Bluetooth enable failed"
        }
    }

    @SuppressLint("MissingPermission")
    private val gattCallback = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(gatt: BluetoothGatt?, status: Int, newState: Int) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                runOnUiThread { statusTextView.text = "Connected" }
                gatt?.discoverServices()
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                runOnUiThread { statusTextView.text = "Disconnected" }
            }
        }

        override fun onServicesDiscovered(gatt: BluetoothGatt?, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                val service = gatt?.getService(serviceUUID)
                val characteristic = service?.getCharacteristic(characteristicUUID)
                gatt?.setCharacteristicNotification(characteristic, true)
            }
        }

        override fun onCharacteristicChanged(gatt: BluetoothGatt?, characteristic: BluetoothGattCharacteristic?) {
            if (characteristic?.uuid == characteristicUUID) {
                val receivedData = String(characteristic.value)
                runOnUiThread { receiveTextView.text = "Received: $receivedData" }
            }
        }
    }

    @SuppressLint("MissingPermission")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        statusTextView = findViewById(R.id.statusTextView)
        receiveTextView = findViewById(R.id.receiveTextView)

        val forwardButton: Button = findViewById(R.id.forwardButton)
        val backButton: Button = findViewById(R.id.backButton)
        val leftButton: Button = findViewById(R.id.leftButton)
        val rightButton: Button = findViewById(R.id.rightButton)

        forwardButton.setOnClickListener { sendCommand("W") }
        backButton.setOnClickListener { sendCommand("S") }
        leftButton.setOnClickListener { sendCommand("A") }
        rightButton.setOnClickListener { sendCommand("D") }

        if (bluetoothAdapter == null || !bluetoothAdapter!!.isEnabled) {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            enableBtLauncher.launch(enableBtIntent)
        } else {
            checkLocationPermission()
        }
    }

    private fun checkLocationPermission() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.ACCESS_FINE_LOCATION), REQUEST_LOCATION_PERMISSION)
        } else {
            connectToDevice()
        }
    }

    @SuppressLint("MissingPermission")
    private fun connectToDevice() {
        val device = bluetoothAdapter?.getRemoteDevice(targetDeviceAddress)
        if (device != null) {
            bluetoothGatt = device.connectGatt(this, false, gattCallback)
        } else {
            statusTextView.text = "Device not found"
        }
    }

    @SuppressLint("MissingPermission")
    private fun sendCommand(command: String) {
        val service = bluetoothGatt?.getService(serviceUUID)
        val characteristic = service?.getCharacteristic(characteristicUUID)
        characteristic?.value = command.toByteArray()
        bluetoothGatt?.writeCharacteristic(characteristic)
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_LOCATION_PERMISSION) {
            if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                connectToDevice()
            } else {
                statusTextView.text = "Location permission denied"
            }
        }
    }
}