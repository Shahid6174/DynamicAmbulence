'use client';
import {useState} from "react"


export default function Sidebar({section,setSection}) {
   



  const menuItems = [
    { icon: 'bi-speedometer2', text: 'Dashboard', route: '#dashboard' },
    { icon: 'bi-people-fill', text: 'Patients', route: '#patients' },
    { icon: 'bi-hospital-fill', text: 'Manage Hospitals', route: '#hospitals' },
    { icon: 'bi-broadcast-pin', text: 'Distress Center', route: '#distress' },
    { icon: 'bi-bar-chart-line-fill', text: 'Performance & Ratings', route: '#performance' },
    { icon: 'bi-person-gear', text: 'User Access Control', route: '#users' },
    { icon: 'bi-truck-front-fill', text: 'Ambulance Tracker', route: '#ambulance-tracker' },

];

  return (
    <aside id="sidebar" className="bg-light vh-100 shadow-sm" style={{ width: '200px' }}>
      <nav className="p-2">
        <ul className="nav flex-column">
          {menuItems.map((item, index) => (
            <li key={index} className="nav-item mb-2">
              <p
                onClick={() => setSection(index)}
                className={`nav-link d-flex align-items-center rounded small fw-semibold px-3 py-2 transition-all ${
                  section === index
                    ? 'active-nav-item'
                    : 'text-muted hover-bg-light'
                }`}
                style={
                  section === index
                    ? {
                        backgroundColor: '#e7f0ff',
                        color: '#1763d6',
                        boxShadow: '0 0 0 1px #5caaff',
                      }
                    : { cursor: 'pointer' }
                }
              >
                <i className={`bi ${item.icon} me-2`}></i>
                <span style={{ fontSize: '0.875rem' }}>{item.text}</span>
              </p>
            </li>

          ))}
        </ul>
      </nav>

      {/* Custom hover styling */}
      <style jsx>{`
        .hover-bg:hover {
          background-color: #5caaff;
          text-decoration: none;
        }
      `}</style>
    </aside>
  );
}
