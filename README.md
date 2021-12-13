# Cardiac Operating System

<p align="center">
  <img src="./Images/logo.png" width="70%" >
</p>

# Table of contents:
 
- [Cardiac Operating System](#cardiac-operating-system)
- [Table of contents:](#table-of-contents)
- [Introduction:](#introduction)
- [Connection Diagram:](#connection-diagram)
  - [Server:](#server)
  - [App Diagram:](#app-diagram)
- [AWS Setup:](#aws-setup)
  - [Wavelength Zones:](#wavelength-zones)
  - [VPC:](#vpc)
  - [Gateway Carrier:](#gateway-carrier)
  - [EC2:](#ec2)
  - [Elastic IP:](#elastic-ip)
  - [Connection:](#connection)
- [Code CI/CD:](#code-cicd)
  - [CodePipeline:](#codepipeline)
  - [Code Source:](#code-source)
  - [CodeDeploy:](#codedeploy)
  - [Review:](#review)
  - [Check Deployment:](#check-deployment)
- [App UI:](#app-ui)
  - [**Login Screen**:](#login-screen)
  - [**Monitor Tab**:](#monitor-tab)
  - [**Summary**:](#summary)
  - [**Report**:](#report)
  - [**Additional notes**:](#additional-notes)
  - [**Past Reports**:](#past-reports)
  - [**ECG Details**:](#ecg-details)
  - [**Historical Tab**:](#historical-tab)
  - [**Simulator Tab**:](#simulator-tab)
      - [Functionality:](#functionality)

# Introduction:

Smart telemedicine platform, based on IoT devices that provide Vital signs and historical values. This to transform the way telemedicine is provided and solve the biggest problem in relation to distance check-ups, which is taking biometrics. Our current version consists of an Oxygen Saturation Thimble, an ECG sensor and a contactless Thermometer that can send real time information to a Teams application. This allowing distance consultations and follow ups to become more seamless and to grab a real biometrics and vital signs while doing them.

# Connection Diagram:

## Server:

<img src="./Images/diagram.png">

## App Diagram:

<img src="./Images/Cloud/general.png">

# AWS Setup:

To make the wavelength network work we followed the following AWS documentation, however we will show some important points that must be followed to correctly configure everything.

https://docs.aws.amazon.com/wavelength/latest/developerguide/get-started-wavelength.html

## Wavelength Zones:

We must have access to the appropriate wavelength zone, so check that it is activated as we can see in the image.

<img src="./Images/Cloud/awswave.png">

The test will be carried out with the NOVA platform, on a cell phone connected to the zone: us-west-2-wl1-sfo-wlz-1

<img src="./Images/Cloud/wavezone.png">

## VPC: 

The VPC settings must be as follows so that you can connect to the Wavelength network successfully.

<img src="./Images/Cloud/awsvpc.png">

It is very important for the IPv4 CIDR to be:
10.0.0.0/24

## Gateway Carrier:

In this step, the important thing will be to select the correct AWS Wavelength region in the subnet.

<img src="./Images/Cloud/awsvpc.png">

## EC2:

The configuration of the EC2 is very important that the following parameters are maintained:

Instance type:

- t3.medium, t3.xlarge, and r5.2xlarge for applications that need cost effective general purpose compute; 

- g4dn.2xlarge for applications that need GPUs such as game streaming and ML inference at the edge.

Tags:

Although it is possible to configure this later, place the following tags at the end of the creation of the instance, these will serve to carry out the deployment of the app later.

<img src="./Images/Cloud/awstags.png">

Whatever configuration you want to do outside of the instance type there are no restrictions.

<img src="./Images/Cloud/awsec2.png">

To make the correct deployment of the application it is important that the IAM role of our instance has the following permissions enabled.

<img src="./Images/Cloud/awsiam.png">

## Elastic IP:

One of the parts that can give more problems if it is not done correctly will be to assign the Elastic IP to our instance.

<img src="./Images/Cloud/awselastic.png">

We just have to click on assign as the box shows.

<img src="./Images/Cloud/awsassign.png">

## Connection:

If everything we did was done correctly, we should be able to access our instance from the option that I show below.

Thanks to the fact that we will use a code deployment pipeline, it is all the access we need.

<img src="./Images/Cloud/awsconnect.png">

# Code CI/CD:

## CodePipeline:

In AWS CodePipeline create a pipeline with the following characteristics.

<img src="./Images/Code/1.png">

## Code Source:

Select Github or your preferred code repository as a base.

<img src="./Images/Code/2.png">

## CodeDeploy:

When configuring the deployment, it is very important to respect the tags that we use in the EC2 configuration so that the platform detects it correctly.

<img src="./Images/Code/4-1.png">

If everything worked correctly we should see the following.

<img src="./Images/Code/4.png">

## Review:

Inside the code folder, check in detail the scripts that the machine will run to do all the deployment, the deployment will run on EC2 as an Apache server.

[Code/scripts](https://github.com/altaga/../../../WebPage/scripts)

In turn, the details of the deployment are defined in the file.

[Code/appspec.yml](https://github.com/altaga/../../../WebPage/appspec.yml)

Since everything worked correctly we should see that the pipeline is like this.

<img src="./Images/Code/awsdeploy.png">

## Check Deployment:

CarrierIP:
[155.146.24.214](http://155.146.24.214)

Now we can check that our application is correctly deployed when entering a device with 5G thanks to the NOVA platform.

<img src="./Images/Cloud/scupdeploy.png">

Here we can see how the page is not accessible if we are not connected to a Verizon 5G zone, in this case the pc is connected to a normal local internet.

<img src="./Images/Cloud/scupdeployfail.png">

# App UI:

The tests were being carried out on a cell phone.

[Samsung Galaxy S20+ 5G (SM-G986U)](https://www.samsung.com/us/business/products/mobile/phones/galaxy-s/galaxy-s20-plus-5g-512gb-unlocked-sm-g986uzkexaa/)

With a speed of 1246Mbps

<img src="./Images/Cloud/speed.png" width="33%">

The app for the test will be reviewed only the mobile version since it is the access we have from NOVA. Although the app is a PWA so it can work on a desktop connected to a Wavelength zone too if you want to try it.

## **Login Screen**:

The login screen gives access to the platform by entering the following credentials.

Login Credentials:

    Email:drgregoryhouse@gmail.com
    Password:toor

**Desktop**:

<img src="./Images/desktop/login-desk.png">

**Mobile**:

<img src="./Images/mobile/login-mob.png" width="32%">

## **Monitor Tab**:

The platform communicates with IoT devices through MQTT, which allows us to receive the data and graph it with the lowest possible latency.

ECG: 

The data received from the holter monitor is processed in real time on the page through a filter to be able to see the EKG graph correctly.

<img src="./Images/ecg.png">

Oxygen Saturation:

The data received by the oxygen saturation sensor are:

- BPM.
- blood's SatO2.
- Plethysmography Curve

<img src="./Images/sat.png">

Temperature:

This sensor takes the temperature of the patient's hand.

<img src="https://i.stack.imgur.com/HK7op.gif" width="1000" />

To calculate the real temperature of the body, a multivariable linear regression model was performed to obtain an equation that would relate the temperature of the back of the hand and the ambient temperature, to obtain the real internal temperature of the body.

<img src="https://i.ibb.co/Rgm108g/image.png" width="1000">

The data received by the platform is the calculated core body temperature.

<img src="./Images/temp.png">

Tab Explorer:

This section helps us to navigate between the 5 tabs that the monitor consists of.

<img src="./Images/explorer.png">

Patient Data:

In this section we can see the patient's data, in addition to having a convenient button to switch between the English system and the international system for temperature.

<img src="./Images/data.png">

**Desktop**:

<img src="./Images/desktop/monitor-desk.png">

**Mobile**:

<img src="./Images/mobile/monitor-mob.png" width="32%"><img src="./Images/mobile/monitor2-mob.png" width="32%"><img src="./Images/mobile/monitor3-mob.png" width="32%">

## **Summary**:

In the summary we can see a summary of the general data of the patient and manipulate them as the doctor sees fit, all these data will be permanently modified once the submit button is clicked.

**Desktop**:

<img src="./Images/desktop/summary-desk.png">

**Mobile**:

<img src="./Images/mobile/summary-mob.png" width="32%">

## **Report**: 

In this section the doctor will be able to fill in the data obtained during the consultation, the vital signs data are filled in automatically while the data is received. 

**Desktop**:

<img src="./Images/desktop/report-desk.png">

**Mobile**:

<img src="./Images/mobile/report-mob.png" width="32%">

## **Additional notes**:

All the data that cannot be added through the report can be filled in this section, to be able to add tables, images or any type of additional data that is required.

**Desktop**:

<img src="./Images/desktop/notes-desk.png">

**Mobile**:

<img src="./Images/mobile/editor-mob.png" width="32%">

## **Past Reports**:

In order to review the previous reports of any patient, once you select the patient, the dates of the previous reports will automatically appear so that you can display them.

**Desktop**:

<img src="./Images/desktop/pastreport-desk.png">

**Mobile**:

<img src="./Images/mobile/pastreport-mob.png" width="32%">

## **ECG Details**:

In the case of the ECG details, we will have to be receiving ECG data (you can activate the simulator data), once we receive at least 10 - 15 seconds of data, the ECG symbol will turn from gray to red, this means that Once the pressures are done, we will send the data to an ECG analysis API to obtain relevant data for a doctor or physician/cardiologist.

**Desktop**:

<img src="./Images/desktop/ecg-desk.png">

**Mobile**:

<img src="./Images/mobile/ecg-mob.png" width="32%">

## **Historical Tab**:

This tab has the function of displaying the stored data of the patients in their consultations, which is downloaded from a database that is updated as the patient has consultations.

To display the data we have to select the patient and then move on the calendar to review the data of the day that we need to display.

**Desktop**:

<img src="./Images/desktop/historical-desk.png">

**Mobile**:

<img src="./Images/mobile/historical1-mob.png" width="32%"><img src="./Images/mobile/historical-mob.png" width="32%">

## **Simulator Tab**:

As this project is based on IoT 

This tab has the function of simulating the data in real time of a patient during a consultation, in order to use it, we will select a patient from the drop-down list and activate the sensors that we want to simulate, once the sensors that we want to simulate are in color, we will return to the monitor tab to be able to see the real-time data of the patient.

To Test this application enter here first!:

Pending LINK!

With these login credentials:

    Email:drgregoryhouse@gmail.com
    Password:toor

#### Functionality:

You can follow the next video to use the application:

Video: Click on the image
[![DEMO](./Images/click-here-button.png)](https://youtu.be/hMcxbMPezFI)

Or the following Instructions:

To test the simulator, we must log in to the page first:

<img src="https://i.ibb.co/F6jrfcq/1.png">

Then, we will select one of the patients, in this case Victor Altamirano. We can see that no data appears yet.

<img src="https://i.ibb.co/ZxRvYpK/2.png">

Next, we go to the simulator tab. Select the patient we want to simulate and click on the buttons on the right, this will simulate one of the sensors.

<img src="https://i.ibb.co/kBRzXhS/3.png">

We will activate the 3 sensors to show how they arrive at the platform at the same time.

<img src="https://i.ibb.co/ZhJgdwd/4.png">

To see this go back to the monitor tab.

<img src="https://i.ibb.co/1LfnjyF/5.png">

Then deactivate 2 of them to show that now only the temperature arrives!

<img src="https://i.ibb.co/k6Qsgcm/7.png">

Only Temp arrives!

<img src="https://i.ibb.co/pzqPSjR/8.png">

You can now check and play with various combinations of this.

**Desktop**:

<img src="./Images/desktop/simul-desk.png">

**Mobile**:

<img src="./Images/mobile/simulator-mob.png" width="32%">


