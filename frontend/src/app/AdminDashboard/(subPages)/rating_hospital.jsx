'use client';

import { useState } from 'react';
import { Modal, Button } from 'react-bootstrap';

export default function PerformanceRatings(props) {
  const [selected, setSelected] = useState(null);
  const [show, setShow] = useState(false);
  const [hospitals,setHospitals] = useState([
    {
      name: 'Fortis Hospital',
      area: 'Nagarbhavi',
      intake: 34,
      successRate: 92,
      avgDelay: 12,
      rating: 5,
      discharges: 18,
      emergencies: 6,
      aiAccuracy: 89,
      specializations: 'Cardiology, Ortho, Neuro',
      notes: 'Running at full capacity. AI assignments aligned well today.',
    },
    {
      name: 'Aster CMI',
      area: 'Sahakarnagar',
      intake: 20,
      successRate: 81,
      avgDelay: 25,
      rating: 4,
      discharges: 12,
      emergencies: 2,
      aiAccuracy: 77,
      specializations: 'ENT, Kidney, Pulmonology',
    },
  ])

  const openModal = (hospital) => {
    setSelected(hospital);
    setShow(true);
  };

  const closeModal = () => {
    setShow(false);
    setSelected(null);
  };

  return (
    <div className="container-fluid py-4">
      <div className="d-flex justify-content-between align-items-center mb-4">
        <h4 className="fw-bold text-success mb-0">
          <i className="bi bi-bar-chart-line-fill me-2"></i>
          Performance & Ratings
        </h4>
        <span className="text-muted small">
          Evaluate hospital intake, outcomes, delays, and real-time efficiency
        </span>
      </div>

      <div className="card shadow-sm">
        <div className="card-body table-responsive">
          {hospitals.length === 0 ? (
            <p className="text-muted text-center mb-0">No hospital data found.</p>
          ) : (
            <table className="table table-hover align-middle small">
              <thead className="table-light">
                <tr>
                  <th>#</th>
                  <th>Hospital</th>
                  <th>Intake Today</th>
                  <th>Success Rate</th>
                  <th>Avg Delay</th>
                  <th>Rating</th>
                  <th className="text-end">Details</th>
                </tr>
              </thead>
              <tbody>
                {hospitals.map((h, i) => (
                  <tr key={i}>
                    <td>{i + 1}</td>
                    <td>{h.name}</td>
                    <td>{h.intake}</td>
                    <td>{h.successRate}%</td>
                    <td>{h.avgDelay} min</td>
                    <td>
                      {[...Array(5)].map((_, j) => (
                        <i
                          key={j}
                          className={`bi ${
                            j < h.rating ? 'bi-star-fill' : 'bi-star'
                          } text-warning`}
                        ></i>
                      ))}
                    </td>
                    <td className="text-end">
                      <button
                        className="btn btn-sm btn-outline-info"
                        onClick={() => openModal(h)}
                      >
                        <i className="bi bi-info-circle me-1"></i> View
                      </button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </div>
      </div>

      {/* Modal */}
      {selected && (
        <Modal show={show} onHide={closeModal} centered size="lg">
          <Modal.Header closeButton>
            <Modal.Title>{selected.name} – Detailed Report</Modal.Title>
          </Modal.Header>
          <Modal.Body>
            <p><strong>Area:</strong> {selected.area}</p>
            <p><strong>Specializations:</strong> {selected.specializations}</p>
            <p><strong>Total Admissions Today:</strong> {selected.intake}</p>
            <p><strong>Success Rate:</strong> {selected.successRate}%</p>
            <p><strong>Average Delay:</strong> {selected.avgDelay} minutes</p>
            <p><strong>Discharges Today:</strong> {selected.discharges}</p>
            <p><strong>Emergency Intake:</strong> {selected.emergencies}</p>
            <p><strong>AI Assignment Accuracy:</strong> {selected.aiAccuracy}%</p>
            <p><strong>Admin Comments:</strong> <em>{selected.notes || 'N/A'}</em></p>
          </Modal.Body>
          <Modal.Footer>
            <Button variant="secondary" onClick={closeModal}>
              Close
            </Button>
          </Modal.Footer>
        </Modal>
      )}
    </div>
  );
}
